
#pragma once
#include "Win32api_global.h"

class Screen {
public:
	
	int width;                  // ���ڿ��
	int height;                 // ���ڸ߶�
	IUINT32 **framebuffer;      // ���ػ��棺framebuffer[y] ����� y��
	
	Screen() {}
	void Init(int _width, int _height);
	void DrawPixel(int x, int y, UINT32 col);
	void DrawLine(int x1, int y1, int x2, int y2, IUINT32 col);
	
};