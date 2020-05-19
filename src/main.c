#include <windows.h>

#include "types.h"
#include "maths.h"

typedef struct Backbuffer {
	s32 width;
	s32 height;
	void* memory;
	BITMAPINFO bitmapInfo;
} Backbuffer;

static Backbuffer backbuffer;
static b32 running;

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

static void FillTriangle(Backbuffer* buffer, vec2 point0, vec2 point1, vec2 point2, vec3 color)
{
	for (s32 j = 0; j < buffer->height; j++) {
		for (s32 i = 0; i < buffer->width; i++) {
			f32 s, t;
			vec2 point = Vec2i(i, j);
			if (InTriangle(point0, point1, point2, point, &s, &t)) 
				DrawPixel(buffer, point.x, point.y, color);
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
			backbuffer.bitmapInfo.bmiHeader.biHeight = -backbuffer.height;
			backbuffer.bitmapInfo.bmiHeader.biPlanes = 1;
			backbuffer.bitmapInfo.bmiHeader.biBitCount = 32;
			backbuffer.bitmapInfo.bmiHeader.biCompression = BI_RGB;
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
	
	while (running) {
		MSG message;
		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		// Clear backbuffer
		memset(backbuffer.memory, 0, (s64)backbuffer.width * (s64)backbuffer.height * sizeof(s32));

		DrawLine(&backbuffer, Vec2i(30, 30), Vec2i(50, 30), Vec3f(0.f, 0.f, 255.f));
		DrawLine(&backbuffer, Vec2i(30, 30), Vec2i(30, 50), Vec3f(0.f, 255.f, 0.f));
		DrawLine(&backbuffer, Vec2i(30, 30), Vec2i(10, 30), Vec3f(255.f, 0.f, 0.f));
		DrawLine(&backbuffer, Vec2i(30, 30), Vec2i(30, 10), Vec3f(255.f, 255.f, 255.f));

		//DrawTriangle(&backbuffer, Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80), Vec3f(255.f, 0.f, 0.f));
		//DrawTriangle(&backbuffer, Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180), Vec3f(255.f, 255.f, 255.f));
		//DrawTriangle(&backbuffer, Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180), Vec3f(0.f, 255.f, 0.f));

		FillTriangle(&backbuffer, Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80), Vec3f(255.f, 0.f, 0.f));
		FillTriangle(&backbuffer, Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180), Vec3f(255.f, 255.f, 255.f));
		FillTriangle(&backbuffer, Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180), Vec3f(0.f, 255.f, 0.f));

		
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
	return 0;
}