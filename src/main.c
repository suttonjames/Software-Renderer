#include <windows.h>
#include <stdint.h>

#define false 0;
#define true 1;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;
typedef s32 b32;

typedef struct Backbuffer
{
	s32 width;
	s32 height;
	void* memory;
	BITMAPINFO bitmapInfo;
} Backbuffer;

static Backbuffer backbuffer;
static b32 running;

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
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
			backbuffer.bitmapInfo.bmiHeader.biHeight = -backbuffer.height; // top-down
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
		s32* pixel = (s32*)backbuffer.memory;
		for (s32 y = 0; y < backbuffer.height; y++) {
			for (s32 x = 0; x < backbuffer.width; x++) {
				*pixel++ = 0x00FF00;
			}
		}
		
		StretchDIBits(device_context, 
			0, 0, 
			backbuffer.width, backbuffer.height, 
			0, 0, 
			backbuffer.width, backbuffer.height, 
			backbuffer.memory, 
			&backbuffer.bitmapInfo, 
			DIB_RGB_COLORS, 
			SRCCOPY);
	}
	return 0;
}