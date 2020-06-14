#include <windows.h>

#include "types.h"
#include "maths.h"

#include "image.h"
#include "model.h"

typedef struct Backbuffer {
	s32 width;
	s32 height;
	void *memory;
	BITMAPINFO bitmapInfo;
} Backbuffer;

static Backbuffer backbuffer;
static b32 running;
static f32 *zbuffer;
static mat4 viewport;

static void Swap(f32 *xp, f32 *yp)
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

static void DrawLine(Backbuffer *buffer, vec2 v0, vec2 v1, vec3 colour)
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

static void DrawTriangle(Backbuffer *buffer, vec2 v0, vec2 v1, vec2 v2, vec3 colour)
{
	DrawLine(buffer, v0, v1, colour);
	DrawLine(buffer, v1, v2, colour);
	DrawLine(buffer, v2, v0, colour);
}

static int InTriangle(vec2 a, vec2 b, vec2 c, vec2 p, f32 *sp, f32 *tp)
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

static void FillTriangle(Backbuffer *buffer, vec3 point0, vec3 point1, vec3 point2, vec3 colour0, vec3 colour1, vec3 colour2, f32 *zbuffer, f32 intensity)
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
	mat4 mvp_inverse;
	vec3 light;
	Image *diffuse_map;
	Image *normal_map;
	Image *specular_map;
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
	*out_texcoord = in_texcoord;

	vec4 position = Vec4(in_position, 1.0f);
	vec4 clip_coord = Mat4MultiplyVec4(mvp, position);

	return clip_coord;
}

static vec3 FragmentShader(void *varyings_, void *uniforms_)
{
	Varyings *varyings = (Varyings*)varyings_;
	Uniforms *uniforms = (Uniforms*)uniforms_;

	vec2 in_texcoord = varyings->in_texcoord;

	mat4 mvp = uniforms->mvp;
	mat4 mvp_inverse = uniforms->mvp_inverse;
	vec3 light = uniforms->light;
	Image *diffuse_map = uniforms->diffuse_map;
	Image *normal_map = uniforms->normal_map;
	Image *specular_map = uniforms->specular_map;

	// transfor normal
	vec3 normal = SampleTexture(normal_map, in_texcoord);
	normal.x = normal.r / 255.0f * 2.0f - 1.0f;
	normal.y = normal.g / 255.0f * 2.0f - 1.0f;
	normal.z = normal.b / 255.0f * 2.0f - 1.0f;
	vec4 normal_4f = Vec4(normal, 1.0f);
	normal_4f = Mat4MultiplyVec4(mvp, normal_4f);
	normal = Vec3Normalise(Vec3(normal_4f));

	// transform light
	vec4 light_4f = Vec4(light, 1.0f);
	light_4f = Mat4MultiplyVec4(mvp, light_4f);
	light = Vec3Normalise(Vec3(light_4f));

	// reflected = 2 * normal * dot(normal, light) - light
	float intensity = Vec3Dot(normal, light);
	vec3 reflected = Vec3Scale(normal, intensity * 2.0f);
	reflected = Vec3Normalise(Vec3Minus(reflected, light));

	// specular factor
	vec3 spec = SampleTexture(specular_map, in_texcoord);
	float specular = spec.b;
	float base = max(reflected.z, 0.0f);
	specular = (float)pow(base, specular);

	// diffuse factor
	float diffuse = max(intensity, 0.0f);

	vec3 colour = SampleTexture(diffuse_map, in_texcoord);
	colour.r = 5.0f + colour.r * (diffuse + 0.6f * specular);
	colour.g = 5.0f + colour.g * (diffuse + 0.6f * specular);
	colour.b = 5.0f + colour.b * (diffuse + 0.6f * specular);
	colour.r = min(255.0f, colour.r);
	colour.g = min(255.0f, colour.g);
	colour.b = min(255.0f, colour.b);

	return colour;
}

static vec2 Vec2Interpolate(vec2 v[3], f32 s, f32 t) {
	vec2 result;
	result.x = ((1.0f - s - t) * v[0].x + s * v[1].x + t * v[2].x);
	result.y = ((1.0f - s - t) * v[0].y + s * v[1].y + t * v[2].y);
	return result;
}

static void InterpolateVaryings(f32 s, f32 t, void *varyings_) {
	Varyings *varyings = (Varyings*)varyings_;
	vec2 *out_texcoords = varyings->out_texcoords;
	varyings->in_texcoord = Vec2Interpolate(out_texcoords, s, t);
}

static void Draw(Backbuffer *buffer, Program *program)
{
	void *varyings = program->varyings;
	void *uniforms = program->uniforms;
	vec3 screen_coords[3];

	for (s32 i = 0; i < 3; i++) {
		vec4 clip_coord = VertexShader(i, varyings, uniforms);
		vec4 coord = Vec4f(clip_coord.x / clip_coord.w, clip_coord.y / clip_coord.w, clip_coord.z / clip_coord.w, clip_coord.w / clip_coord.w);
		vec4 ndc_coord = Mat4MultiplyVec4(viewport, coord);
		screen_coords[i].x = ndc_coord.x;
		screen_coords[i].y = ndc_coord.y;
		screen_coords[i].z = ndc_coord.z;
	}
	s32 min_x = buffer->width - 1, min_y = buffer->height - 1;
	s32 max_x = 0, max_y = 0;

	min_x = min(screen_coords[2].x, min(screen_coords[1].x, min(screen_coords[0].x, min_x)));
	min_y = min(screen_coords[2].y, min(screen_coords[1].y, min(screen_coords[0].y, min_y)));
	max_x = max(screen_coords[2].x, max(screen_coords[1].x, max(screen_coords[0].x, max_x)));
	max_y = max(screen_coords[2].y, max(screen_coords[1].y, max(screen_coords[0].y, max_y)));

	min_x = max(0, min_x);
	min_y = max(0, min_y);
	max_x = min(buffer->width - 1, max_x);
	max_y = min(buffer->height - 1, max_y);

	for (s32 j = min_y; j < max_y; j++) {
		for (s32 i = min_x; i < max_x; i++) {
			vec2 point = Vec2i(i, j);
			vec2 point0 = Vec2i(screen_coords[0].x, screen_coords[0].y);
			vec2 point1 = Vec2i(screen_coords[1].x, screen_coords[1].y);
			vec2 point2 = Vec2i(screen_coords[2].x, screen_coords[2].y);
			f32 s, t;
			if (InTriangle(point0, point1, point2, point, &s, &t)) {
				f32 depth = (1.0f - s - t) * screen_coords[0].z + s * screen_coords[1].z + t * screen_coords[2].z;
				if (zbuffer[j * buffer->width + i] < depth) {
					InterpolateVaryings(s, t, varyings);
					vec3 colour = FragmentShader(varyings, uniforms);
					DrawPixel(buffer, point.x, point.y, colour);
					zbuffer[j * buffer->width + i] = depth;
				}
			}
		}
	}
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
			backbuffer.bitmapInfo.bmiHeader.biHeight = backbuffer.height;
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
		//800, 800,
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

	Model *model = LoadModel("assets/african_head.obj");
	Image *diffuse_map = ReadFromTGA("assets/african_head_diffuse.tga");
	Image *normal_map = ReadFromTGA("assets/african_head_nm.tga");
	Image *specular_map = ReadFromTGA("assets/african_head_spec.tga");

	vec3 eye = Vec3f(1.0f, 1.0f, 3.0f);
	vec3 centre = Vec3f(0.0f, 0.0f, 0.0f);
	vec3 up = Vec3f(0.0f, 1.0f, 0.0f);

	mat4 model_view = LookAt(eye, centre, up);
	f32 coeff = -1.0f / Vec3Length(Vec3Minus(centre, eye));
	mat4 projection = Projection(coeff);
	mat4 mvp = Mat4Multiply(projection, model_view);
	mat4 mvp_inverse = Mat4InverseTranspose(mvp);

	vec3 light = Vec3f(1.0f, 1.0f, 1.0f);

	Varyings varyings;
	Uniforms uniforms;
	program.varyings = &varyings;
	program.uniforms = &uniforms;
	uniforms.mvp = mvp;
	uniforms.mvp_inverse = mvp_inverse;
	uniforms.light = light;
	uniforms.diffuse_map = diffuse_map;
	uniforms.normal_map = normal_map;
	uniforms.specular_map = specular_map;
	
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

		for (s32 i = 0; i < model->num_faces; i++) {
			for (s32 j = 0; j < 3; j++) {
				varyings.in_positions[j] = model->positions[i * 3 + j];
				varyings.in_texcoords[j] = model->texcoords[i * 3 + j];
			}
			Draw(&backbuffer, &program);
		}

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

	FreeModel(model);
	free(diffuse_map);
	free(normal_map);
	free(specular_map);
	return 0;
}