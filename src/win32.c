#include <windows.h>

#include "platform.h"
#include "image.h"
#include "model.h"
#include "draw.h"

static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = -1;
	switch (message) {
		case WM_CLOSE:
		case WM_DESTROY:
		case WM_QUIT: 
		{
			platform.running = false;
			PostQuitMessage(0);
		} break;
		case WM_SIZE:
		{
			RECT rect;
			GetClientRect(window, &rect);
			platform.backbuffer.width = rect.right - rect.left;
			platform.backbuffer.height = rect.bottom - rect.top;

			s32 size = platform.backbuffer.width * platform.backbuffer.height * sizeof(s32);

			if (platform.backbuffer.memory)
				VirtualFree(platform.backbuffer.memory, 0, MEM_RELEASE);
			
			platform.backbuffer.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			platform.backbuffer.bitmapInfo.bmiHeader.biSize = sizeof(platform.backbuffer.bitmapInfo.bmiHeader);
			platform.backbuffer.bitmapInfo.bmiHeader.biWidth = platform.backbuffer.width;
			platform.backbuffer.bitmapInfo.bmiHeader.biHeight = platform.backbuffer.height;
			platform.backbuffer.bitmapInfo.bmiHeader.biPlanes = 1;
			platform.backbuffer.bitmapInfo.bmiHeader.biBitCount = 32;
			platform.backbuffer.bitmapInfo.bmiHeader.biCompression = BI_RGB;

			if (platform.backbuffer.zbuffer)
				VirtualFree(platform.backbuffer.zbuffer, 0, MEM_RELEASE);

			platform.backbuffer.zbuffer = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			platform.viewport = Viewport(0, 0, platform.backbuffer.width, platform.backbuffer.height);

		} break;
		default:
			result = DefWindowProc(window, message, wParam, lParam);
		}
	return result;
}


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	platform.running = true;
	
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
	platform.program.varyings = &varyings;
	platform.program.uniforms = &uniforms;
	uniforms.mvp = mvp;
	uniforms.mvp_inverse = mvp_inverse;
	uniforms.light = light;
	uniforms.diffuse_map = diffuse_map;
	uniforms.normal_map = normal_map;
	uniforms.specular_map = specular_map;
	
	while (platform.running) {
		MSG message;
		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		// Clear backbuffer
		memset(platform.backbuffer.memory, 0, (s64)platform.backbuffer.width * (s64)platform.backbuffer.height * sizeof(s32));
		// Clear zbuffer
		memset(platform.backbuffer.zbuffer, 0, (s64)platform.backbuffer.width * (s64)platform.backbuffer.height * sizeof(f32));

		for (s32 i = 0; i < model->num_faces; i++) {
			for (s32 j = 0; j < 3; j++) {
				varyings.in_positions[j] = model->positions[i * 3 + j];
				varyings.in_texcoords[j] = model->texcoords[i * 3 + j];
			}
			Draw(&platform.backbuffer, &platform.program, platform.viewport);
		}

		StretchDIBits(device_context, 
			0, 0, 
			platform.backbuffer.width, platform.backbuffer.height, 
			0, 0, 
			platform.backbuffer.width, platform.backbuffer.height, 
			platform.backbuffer.memory, 
			&platform.backbuffer.bitmapInfo, 
			DIB_RGB_COLORS, 
			SRCCOPY
		);
	}

	FreeModel(model);
	FreeImage(diffuse_map);
	FreeImage(normal_map);
	FreeImage(specular_map);
	return 0;
}