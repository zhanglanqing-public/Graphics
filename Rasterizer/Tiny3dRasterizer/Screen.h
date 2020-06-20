
#pragma once
#include "Win32api_global.h"

class Screen {
public:
	
	int width;                  // 窗口宽度
	int height;                 // 窗口高度
	IUINT32 **framebuffer;      // 像素缓存：framebuffer[y] 代表第 y行
	
	Screen() {}
	void Init(int _width, int _height);
	void DrawPixel(int x, int y, UINT32 col);
	void DrawLine(int x1, int y1, int x2, int y2, IUINT32 col);
	
};