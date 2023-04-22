#include <windows.h>
#include <algorithm>
#include <cmath>
#include <string>
#include <map>
#include <filesystem>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

static std::atomic<bool> running = true;

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

#include "util.h"
#include "renderer.h"
#include "asset_loader.h"

void paintWindow(HWND hWnd);
void messageLoop(HWND hWnd);

LRESULT CALLBACK mainCallbackFunc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
	switch (uMsg) {
	case WM_CLOSE:
	case WM_DESTROY: {
		running = false;
		PostQuitMessage(0);
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
		paintWindow(hWnd);
	}

	default:
		result = DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return result;
}

void messageLoop(HWND hWnd) {
	MSG message;
	while (running) {
		while (GetMessage(&message, hWnd, 0, 0)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

	}
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

	std::thread messageThread(messageLoop,mainWindow);
	std::chrono::duration<int, std::ratio<1, 1000>> fps;

	while (running) {

		
	}

	messageThread.join();
}

void paintWindow(HWND hWnd) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	clearScreen(0x886622);
	StretchDIBits(hdc, 0, 0, renderBuffer.width, renderBuffer.height, 0, 0, renderBuffer.width, renderBuffer.height, renderBuffer.memory, &renderBuffer.bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
	LoadedSprite spriteVixel = resources.at("vixel.bmp");
	spriteVixel = loadTransparency(hdc, "vixel.bmp");
	StretchBlt(hdc, renderBuffer.width / 2 - spriteVixel.bm.bmWidth * 6 / 2, renderBuffer.height / 2 - spriteVixel.bm.bmHeight * 6 / 2, spriteVixel.bm.bmWidth * 6, spriteVixel.bm.bmHeight * 6, spriteVixel.memDC, 0, 0, spriteVixel.bm.bmWidth, spriteVixel.bm.bmHeight, SRCCOPY);
	EndPaint(hWnd, &ps);
}