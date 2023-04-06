#include <utility>
#include "util.h"
#include <windows.h>

void paintWindow(HWND hWnd);

static bool running = true;

struct RenderBuffer {
	void* memory;
	int width, height;

	BITMAPINFO bitmapInfo;
};

struct GlobalProperties {
	struct NativeRes {
		int x = 1280;
		int y = 720;
	};


	NativeRes nativeRes;
};

static RenderBuffer renderBuffer;
static GlobalProperties globalProperties;

#include "platform_common.cpp"
#include "main_game.cpp"
#include "renderer.h"
#include "assets.h"


LRESULT CALLBACK mainCallbackFunc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
	switch (uMsg) {
	case WM_CLOSE:
	case WM_DESTROY: {
		running = false;
	} break;

	case WM_SIZE: {
		RECT rect;
		GetClientRect(hWnd, &rect);
		renderBuffer.width = rect.right - rect.left;
		renderBuffer.height = rect.bottom - rect.top;

		int bufferSize = renderBuffer.width * renderBuffer.height * sizeof(u32);

		if (renderBuffer.memory) VirtualFree(renderBuffer.memory, 0, MEM_RELEASE);
		renderBuffer.memory = VirtualAlloc(0, bufferSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		renderBuffer.bitmapInfo.bmiHeader.biSize = sizeof(renderBuffer.bitmapInfo.bmiHeader);
		renderBuffer.bitmapInfo.bmiHeader.biWidth = renderBuffer.width;
		renderBuffer.bitmapInfo.bmiHeader.biHeight = renderBuffer.height;
		renderBuffer.bitmapInfo.bmiHeader.biPlanes = 1;
		renderBuffer.bitmapInfo.bmiHeader.biBitCount = 32;
		renderBuffer.bitmapInfo.bmiHeader.biCompression = BI_RGB;

	} break;

	case WM_PAINT: {
		clearScreen(0x886622);
		paintWindow(hWnd);
	}

	default:
		result = DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return result;
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	//Create a window class
	WNDCLASS mainWindowClass = {};
	mainWindowClass.style = CS_HREDRAW || CS_VREDRAW;
	mainWindowClass.lpszClassName = "Main Window Class";
	mainWindowClass.lpfnWndProc = mainCallbackFunc;

	//Register the window class with its pointer
	RegisterClass(&mainWindowClass);
		
	//Pass it to the funtion which will create the window
	HWND mainWindow = CreateWindow(mainWindowClass.lpszClassName, "First win32 App", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, globalProperties.nativeRes.x, globalProperties.nativeRes.y, 0, 0, hInstance, 0);
	HDC hdc = GetDC(mainWindow);
	
	initResources(hdc);

	Input input = {};

	while (running) {
		//Input
		MSG message;

		for (int i = 0; i < BUTTON_COUNT; i++) {
			input.buttons[i].changed = false;
		}

		while (PeekMessage(&message, mainWindow, 0, 0, PM_REMOVE)) {

			switch (message.message) {
				case WM_KEYUP:
				case WM_KEYDOWN: {
					u32 vkCode = (u32)message.wParam;
					bool isDown = ((message.lParam & (1 << 31)) == 0);
					switch (vkCode)
					{
						case VK_UP: {
							input.buttons[BUTTON_UP].isDown = isDown;
							input.buttons[BUTTON_UP].changed = true;
						} break;
						
						case VK_DOWN: {

						} break;

						default:
							break;
					}
				} break;

				default: {
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
			}
		}
	}
}

void paintWindow(HWND hWnd) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	StretchDIBits(hdc, 0, 0, renderBuffer.width, renderBuffer.height, 0, 0, renderBuffer.width, renderBuffer.height, renderBuffer.memory, &renderBuffer.bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
	LoadedSprite spriteVixel = resources.at("vixel.bmp");
	StretchBlt(hdc, renderBuffer.width / 2 - spriteVixel.bm.bmWidth * 6 / 2, renderBuffer.height / 2 - spriteVixel.bm.bmHeight * 6 / 2, spriteVixel.bm.bmWidth * 6, spriteVixel.bm.bmHeight * 6, spriteVixel.memDC, 0, 0, spriteVixel.bm.bmWidth, spriteVixel.bm.bmHeight, SRCCOPY);
	EndPaint(hWnd, &ps);
}