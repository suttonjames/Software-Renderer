#include <windows.h>

#include "types.h"
#include "maths.h"

#include "image.h"
#include "model.h"

typedef struct Backbuffer {
	s32 width;
	s32 height;
	void* memory;
	BITMAPINFO bitmapInfo;
} Backbuffer;

static Backbuffer backbuffer;
static b32 running;
static f32 *zbuffer;
static mat4 viewport;

static void Swap(f32* xp, f32* yp)
{
	f32 temp = *xp;
	*xp = *yp;
	*yp = temp;
}

static void DrawPixel(Backbuffer* buffer, s32 x, s32 y, vec3 colour)
{
	if (x < 0 || y < 0 || x >= buffer->width || y >= buffer->height) 
		return;

	s32* pixel = (s32*)buffer->memory + ((s64)x + (s64)y * buffer->width);
	*pixel = (((s32)colour.r << 16) | ((s32)colour.g << 8) | (s32)colour.b);
}

static void DrawLine(Backbuffer* buffer, vec2 v0, vec2 v1, vec3 colour)
{
	s32 dx = abs(v1.x - v0.x);
	s32 dy = abs(v1.y - v0.y);

	if (dx > dy) {
		if (v0.x > v1.x) {
			Swap(&v0.x, &v1.x);
			Swap(&v0.y, &v1.y);
		}
		for (s32 x = v0.x; x <= v1.x; x++) {
			f32 t = (x - v0.x) / (v1.x - v0.x);
			s32 y = (s32)(v0.y + (v1.y - v0.y) * t);
			DrawPixel(buffer, x, y, colour);
		}
	} else {
		if (v0.y > v1.y) {
			Swap(&v0.y, &v1.y);
			Swap(&v0.x, &v1.x);
		}
		for (s32 y = v0.y; y <= v1.y; y++) {
			f32 t = (y - v0.y) / (v1.y - v0.y);
			s32 x = (s32)(v0.x + (v1.x - v0.x) * t);
			DrawPixel(buffer, x, y, colour);
		}
	}
}

static void DrawTriangle(Backbuffer* buffer, vec2 v0, vec2 v1, vec2 v2, vec3 colour)
{
	DrawLine(buffer, v0, v1, colour);
	DrawLine(buffer, v1, v2, colour);
	DrawLine(buffer, v2, v0, colour);
}

static int InTriangle(vec2 a, vec2 b, vec2 c, vec2 p, f32* sp, f32* tp)
{
	vec2 AB = Vec2Minus(b, a);
	vec2 AC = Vec2Minus(c, a);
	vec2 AP = Vec2Minus(p, a);
	f32 s, t;

	s32 denom = AB.x * AC.y - AB.y * AC.x;
	if (denom == 0) 
		return false;

	s = (AC.y * AP.x - AC.x * AP.y) / (f32)denom;
	t = (AB.x * AP.y - AB.y * AP.x) / (f32)denom;

	*sp = s;
	*tp = t;

	return (s >= 0 && t >= 0 && s + t <= 1);
}

static void FillTriangle(Backbuffer* buffer, vec3 point0, vec3 point1, vec3 point2, vec3 colour0, vec3 colour1, vec3 colour2, f32 *zbuffer, f32 intensity)
{
	s32 min_x = buffer->width - 1, min_y = buffer->height - 1;
	s32 max_x = 0, max_y = 0;

	min_x = min(point2.x, min(point1.x, min(point0.x, min_x)));
	min_y = min(point2.y, min(point1.y, min(point0.y, min_y)));
	max_x = max(point2.x, max(point1.x, max(point0.x, max_x)));
	max_y = max(point2.y, max(point1.y, max(point0.y, max_y)));

	min_x = max(0, min_x);
	min_y = max(0, min_y);
	max_x = min(buffer->width - 1, max_x);
	max_y = min(buffer->height - 1, max_y);

	for (s32 j = min_y; j < max_y; j++) {
		for (s32 i = min_x; i < max_x; i++) {
			f32 s, t;
			vec2 point = Vec2i(i, j);
			vec2 point02 = Vec2i(point0.x, point0.y);
			vec2 point12 = Vec2i(point1.x, point1.y);
			vec2 point22 = Vec2i(point2.x, point2.y);
			if (InTriangle(point02, point12, point22, point, &s, &t)) {
				f32 z = (1.0f - s - t) * point0.z + s * point1.z + t * point2.z;
				vec3 colour;
				colour.r = ((1.0f - s - t) * colour0.r + s * colour1.r + t * colour2.r) * intensity;
				colour.g = ((1.0f - s - t) * colour0.g + s * colour1.g + t * colour2.g) * intensity;
				colour.b = ((1.0f - s - t) * colour0.b + s * colour1.b + t * colour2.b) * intensity;
				if (zbuffer[j * buffer->width + i] < z) {
					DrawPixel(buffer, point.x, point.y, colour);
					zbuffer[j * buffer->width + i] = z;
				}
				
			}
		}
	}
}

typedef struct Varyings {
	// input vertex shader
	vec3 in_positions[3];
	vec2 in_texcoords[3];

	// output vertex shader
	vec2 out_texcoords[3];

	// input fragment shader
	vec2 in_texcoord;
} Varyings;

typedef struct Uniforms {
	mat4 mvp;
} Uniforms;

typedef struct Program {
	void *varyings;
	void *uniforms;
} Program;

static Program program;

static vec4 VertexShader(s32 vertex, void *varyings_, void *uniforms_)
{
	Varyings *varyings = (Varyings*)varyings_;
	Uniforms *uniforms = (Uniforms*)uniforms_;

	vec3 in_position = varyings->in_positions[vertex];
	vec2 in_texcoord = varyings->in_texcoords[vertex];
	mat4 mvp = uniforms->mvp;

	vec2 *out_texcoord = &varyings->out_texcoords[vertex];

	vec4 position = Vec4(in_position, 1.0f);
	vec4 clip_coord = Mat4MultiplyVec4(mvp, position);

	return clip_coord;
}

static vec3 FragmentShader(vec3 colour, void *varyings, void *uniforms)
{

}

static void Draw(Backbuffer *buffer, Program *program)
{
	void *varyings = program->varyings;
	void *uniforms = program->uniforms;
	vec3 screen_coords[3];
	vec3 colours[3];
	f32 intensity = 1.0f;

	for (s32 i = 0; i < 3; i++) {
		vec4 clip_coord = VertexShader(i, varyings, uniforms);
		vec4 coord = Vec4f(clip_coord.x / clip_coord.w, clip_coord.y / clip_coord.w, clip_coord.z / clip_coord.w, clip_coord.w / clip_coord.w);
		vec4 ndc_coord = Mat4MultiplyVec4(viewport, coord);
		screen_coords[i].x = ndc_coord.x;
		screen_coords[i].y = ndc_coord.y;
		screen_coords[i].z = ndc_coord.z;

		colours[i] = Vec3f(255.f, 255.f, 255.f);
	}
	FillTriangle(&backbuffer, screen_coords[0], screen_coords[1], screen_coords[2], colours[0], colours[1], colours[2], zbuffer, intensity);
}


static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = -1;
	switch (message) {
		case WM_CLOSE:
		case WM_DESTROY:
		case WM_QUIT: 
		{
			running = false;
			PostQuitMessage(0);
		} break;
		case WM_SIZE:
		{
			RECT rect;
			GetClientRect(window, &rect);
			backbuffer.width = rect.right - rect.left;
			backbuffer.height = rect.bottom - rect.top;

			s32 size = backbuffer.width * backbuffer.height * sizeof(s32);

			if (backbuffer.memory)
				VirtualFree(backbuffer.memory, 0, MEM_RELEASE);
			
			backbuffer.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			backbuffer.bitmapInfo.bmiHeader.biSize = sizeof(backbuffer.bitmapInfo.bmiHeader);
			backbuffer.bitmapInfo.bmiHeader.biWidth = backbuffer.width;
			backbuffer.bitmapInfo.bmiHeader.biHeight = -backbuffer.height;
			backbuffer.bitmapInfo.bmiHeader.biPlanes = 1;
			backbuffer.bitmapInfo.bmiHeader.biBitCount = 32;
			backbuffer.bitmapInfo.bmiHeader.biCompression = BI_RGB;

			if (zbuffer)
				VirtualFree(zbuffer, 0, MEM_RELEASE);

			zbuffer = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			viewport = Viewport(0, 0, backbuffer.width, backbuffer.height);

		} break;
		default:
			result = DefWindowProc(window, message, wParam, lParam);
		}
	return result;
}


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	running = true;
	
	WNDCLASS window_class = {0};

	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc = WindowProc;
	window_class.lpszClassName = L"Win32 Class";

	if (!RegisterClass(&window_class)) 
		return false;

	HWND window = CreateWindow(
		window_class.lpszClassName,
		L"Renderer", 
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, 
		CW_USEDEFAULT, CW_USEDEFAULT, 
		0, 
		0, 
		hInstance, 
		0
	);

	if (!window)
		return false;

	ShowWindow(window, SW_SHOW);
	UpdateWindow(window);

	HDC device_context = GetDC(window);

	Model *model = LoadModel("src/african_head.obj");
	Image *image = ReadFromTGA("src/african_head_diffuse.tga");

	vec3 eye = Vec3f(-1.0f, 1.0f, 3.0f);
	vec3 centre = Vec3f(0.0f, 0.0f, 0.0f);
	vec3 up = Vec3f(0.0f, -1.0f, 0.0f);

	mat4 model_view = LookAt(eye, centre, up);
	f32 coeff = -1.0f / Vec3Length(Vec3Minus(centre, eye));
	mat4 projection = Projection(coeff);
	mat4 MVP = Mat4Multiply(projection, model_view);
	
	while (running) {
		MSG message;
		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		// Clear backbuffer
		memset(backbuffer.memory, 0, (s64)backbuffer.width * (s64)backbuffer.height * sizeof(s32));
		// Clear zbuffer
		memset(zbuffer, 0, (s64)backbuffer.width * (s64)backbuffer.height * sizeof(f32));

		DrawLine(&backbuffer, Vec2i(30, 30), Vec2i(50, 30), Vec3f(0.f, 0.f, 255.f));
		DrawLine(&backbuffer, Vec2i(30, 30), Vec2i(30, 50), Vec3f(0.f, 255.f, 0.f));
		DrawLine(&backbuffer, Vec2i(30, 30), Vec2i(10, 30), Vec3f(255.f, 0.f, 0.f));
		DrawLine(&backbuffer, Vec2i(30, 30), Vec2i(30, 10), Vec3f(255.f, 255.f, 255.f));

		FillTriangle(&backbuffer, Vec3i(10, 70, 1), Vec3i(50, 160, 1), Vec3i(70, 80, 1), Vec3f(255.f, 0.f, 0.f), Vec3f(0.f, 255.f, 0.f), Vec3f(0.f, 0.f, 255.f), zbuffer, 1.0f);
		FillTriangle(&backbuffer, Vec3i(180, 50, 1), Vec3i(150, 1, 1), Vec3i(70, 180, 1), Vec3f(255.f, 255.f, 255.f), Vec3f(255.f, 255.f, 255.f), Vec3f(0.f, 0.f, 255.f), zbuffer, 1.0f);
		FillTriangle(&backbuffer, Vec3i(180, 150, 1), Vec3i(120, 160, 1), Vec3i(130, 180, 1), Vec3f(0.f, 255.f, 0.f), Vec3f(0.f, 255.f, 0.f), Vec3f(0.f, 255.f, 0.f), zbuffer, 1.0f);

		vec3 light = Vec3f(0.f, 0.f, -1.f);

		Varyings varyings;
		Uniforms uniforms;
		program.varyings = &varyings;
		program.uniforms = &uniforms;
		uniforms.mvp = MVP;

		for (s32 i = 0; i < model->num_faces; i++) {
			for (s32 j = 0; j < 3; j++) {
				varyings.in_positions[j] = model->positions[i * 3 + j];
				varyings.in_texcoords[j] = model->texcoords[i * 3 + j];
			}
			Draw(&backbuffer, &program);
		}
#if 0
		for (s32 i = 0; i < model->num_faces; i++) {
			vec3 screen_coords[3];
			vec3 world_coords[3];
			vec3 colours[3];
			vec3 normal;
			f32 intensity;
			for (s32 j = 0; j < 3; j++) {
				vec3 vertex = model->positions[i * 3 + j];
				vec2 uv = model->texcoords[i * 3 + j];

				vec4 clip_coord = Mat4MultiplyVec4(MVP, Vec4(vertex, 1.0f));
				vec4 coord = Vec4f(clip_coord.x / clip_coord.w, clip_coord.y / clip_coord.w, clip_coord.z / clip_coord.w, clip_coord.w / clip_coord.w);
				vec4 ndc_coord = Mat4MultiplyVec4(viewport, coord);

				screen_coords[j].x = ndc_coord.x;
				screen_coords[j].y = ndc_coord.y;
				screen_coords[j].z = ndc_coord.z;

				s32 tex_x = (s32)(uv.x * image->width);
				s32 tex_y = (s32)(uv.y * image->height);
				colours[j] = GetColour(image, tex_y, tex_x);

				world_coords[j] = vertex;
			}

			normal = Vec3Cross(Vec3Minus(world_coords[2], world_coords[0]), Vec3Minus(world_coords[1], world_coords[0]));
			normal = Vec3Normalise(normal);

			intensity = Vec3Dot(normal, light);

			if (intensity > 0) 
				FillTriangle(&backbuffer, screen_coords[0], screen_coords[1], screen_coords[2], colours[0], colours[1], colours[2], zbuffer, intensity);
		}
#endif
		StretchDIBits(device_context, 
			0, 0, 
			backbuffer.width, backbuffer.height, 
			0, 0, 
			backbuffer.width, backbuffer.height, 
			backbuffer.memory, 
			&backbuffer.bitmapInfo, 
			DIB_RGB_COLORS, 
			SRCCOPY
		);
	}

	free(model);
	return 0;
}