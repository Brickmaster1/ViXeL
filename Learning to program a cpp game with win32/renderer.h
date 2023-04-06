#pragma once

#include <algorithm>
#include <cmath>
#include <string>
#include <filesystem>

static void
clearScreen(u32 color) {
	u32* pixel = (u32*)renderBuffer.memory;
	for (int y = 0; y < renderBuffer.height; y++) {
		for (int x = 0; x < renderBuffer.width; x++) {
			*pixel++ = color;
		}
	}
}

static void
drawRect(float x1, float y1, float x2, float y2, u32 color, Scaling scaling, Anchor anchorPoint) {

	switch (scaling)
	{
	case PROPORTIONAL: {
		x1 = std::round(x1 * (float)renderBuffer.width);
		x2 = std::round(x2 * (float)renderBuffer.width);
		y1 = std::round(y1 * (float)renderBuffer.height);
		y2 = std::round(y2 * (float)renderBuffer.height);
		break;
	}
	case INDEPENDENT: {
		x1 = std::round(x1 * (float)globalProperties.nativeRes.x);
		x2 = std::round(x2 * (float)globalProperties.nativeRes.x);
		y1 = std::round(y1 * (float)globalProperties.nativeRes.y);
		y2 = std::round(y2 * (float)globalProperties.nativeRes.y);
		break;
	}
	case DEPENTDENT_X: {
		x1 = std::round(x1 * (float)renderBuffer.width);
		x2 = std::round(x2 * (float)renderBuffer.width);
		y1 = std::round(x1 * (float)renderBuffer.width);
		y2 = std::round(x2 * (float)renderBuffer.width);
		break;
	}

	case DEPENDENT_Y: {
		x1 = std::round(y1 * (float)renderBuffer.height);
		x2 = std::round(y2 * (float)renderBuffer.height);
		y1 = std::round(y1 * (float)renderBuffer.height);
		y2 = std::round(y2 * (float)renderBuffer.height);
		break;
	}

	default:
		break;
	}



	//Convert the grid from bottom left to one of the other anchor options
	switch (anchorPoint)
	{
	case TOP_LEFT: {
		y1 = renderBuffer.height + y1;
		y2 = renderBuffer.height + y2;
		break;
	}
	case TOP_CENTER: {
		x1 = std::round((float)renderBuffer.width / 2 + x1);
		x2 = std::round((float)renderBuffer.width / 2 + x2);
		y1 = renderBuffer.height + y1;
		y2 = renderBuffer.height + y2;
		break;
	}
	case TOP_RIGHT: {
		x1 = renderBuffer.width + x1;
		x2 = renderBuffer.width + x2;
		y1 = renderBuffer.height + y1;
		y2 = renderBuffer.height + y2;
		break;
	}
	case CENTER_LEFT: {
		y1 = std::round((float)renderBuffer.height / 2 + y1);
		y2 = std::round((float)renderBuffer.height / 2 + y2);
		break;
	}
	case CENTER: {
		x1 = std::round((float)renderBuffer.width / 2 + x1);
		x2 = std::round((float)renderBuffer.width / 2 + x2);
		y1 = std::round((float)renderBuffer.height / 2 + y1);
		y2 = std::round((float)renderBuffer.height / 2 + y2);
		break;
	}
	case CENTER_RIGHT: {
		x1 = renderBuffer.width + x1;
		x2 = renderBuffer.width + x2;
		y1 = std::round((float)renderBuffer.height / 2 + y1);
		y2 = std::round((float)renderBuffer.height / 2 + y2);
		break;
	}
	case BOTTOM_LEFT: {
		break;
	}
	case BOTTOM_CENTER: {
		x1 = std::round((float)renderBuffer.width / 2 + x1);
		x2 = std::round((float)renderBuffer.width / 2 + x2);
		break;
	}
	case BOTTOM_RIGHT: {
		x1 = renderBuffer.width + x1;
		x2 = renderBuffer.width + x2;
		break;
	}

	default:
		break;
	}

	x1 = std::clamp((int)x1, 0, renderBuffer.width);
	y1 = std::clamp((int)y1, 0, renderBuffer.height);
	x2 = std::clamp((int)x2, 0, renderBuffer.width);
	y2 = std::clamp((int)y2, 0, renderBuffer.height);

	for (int y = (int)y1; y < (int)y2; y++) {
		u32* pixel = (u32*)renderBuffer.memory + (int)x1 + y * renderBuffer.width;
		for (int x = (int)x1; x < (int)x2; x++) {
			*pixel++ = color;
		}
	}
}

struct LoadedSprite {
	BITMAP bm;
	HDC memDC;
	HBITMAP memBmp;
};

static LoadedSprite
loadSprite(HDC hdc, const char* fileLoc, bool transparentBg) {
	HANDLE hSprite = LoadImage(NULL, fileLoc, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_LOADTRANSPARENT);
	if (hSprite == NULL) {
		MessageBox(NULL, "Could not load image.\n" + GetLastError(), "Error", MB_OK);
	}

	BITMAP bm; //Create a bitmap to store the image in
	GetObject(hSprite, sizeof(bm), &bm); //get bitmap dimension 

	LoadedSprite sprite;

	sprite.bm = bm;
	sprite.memDC = CreateCompatibleDC(hdc); //Make room in memory for the sprite 
	sprite.memBmp = (HBITMAP)SelectObject(sprite.memDC, hSprite); //Select the loaded image and put it into the reserved memory

	loadTransparency(hdc, sprite);

	return sprite;
}

static void
loadTransparency(HDC hdc, LoadedSprite sprite) {
	// Get the color of each pixel behind where the image will be displayed and the color to make the background transparent
	for (int x; x < sprite.bm.bmWidth; x++) {
		for (int y; y < sprite.bm.bmHeight; y++) {
			COLORREF bgColor = GetPixel(hdc, 0, 0);
			SetBkColor(sprite.memDC, bgColor);
		}
	}
}

static void
unloadSprite(auto memDC, auto memBmp) {
	SelectObject(memDC, memBmp); //select memDC
	DeleteDC(memDC); //delete it, we are done with it so we free up some memory and avoid a leak
}

//void
//loadSpriteFromSheet(LPSTR lpFileName, Scaling scaling, Anchor anchorPoint) {
//	StretchDIBits(hdc, 0, 0, renderBuffer.width, renderBuffer.height, 0, 0, renderBuffer.width, renderBuffer.height, renderBuffer.memory, &renderBuffer.bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
//}