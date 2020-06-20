#pragma once

#include "Screen.h"

void Screen::Init(int _width, int _height) {
	width = _width;
	height = _height;
	void *fb = screen_fb;
	int need = sizeof(void*) * (height * 2 + 1024) + width * height * 8;
	char *ptr = (char*)malloc(need + 64);
	char *framebuf;
	int j;
	assert(ptr);
	framebuffer = (IUINT32**)ptr;
	ptr += sizeof(void*) * height * 2;
	ptr += sizeof(void*) * 1024;
	framebuf = (char*)ptr;
	ptr += width * height * 8;
	if (fb != NULL) framebuf = (char*)fb;
	for (j = 0; j < height; j++) {
		framebuffer[j] = (IUINT32*)(framebuf + width * 4 * j);
	}
	
}

void Screen::DrawPixel(int x, int y, UINT32 col) {
	if (((IUINT32)x) < (IUINT32) (width) && ((IUINT32)y) < (IUINT32)(height)) {
		framebuffer[y][x] = col;
	}
}

void Screen::DrawLine(int x1, int y1, int x2, int y2, IUINT32 c) {
	int x, y, rem = 0;
	if (x1 == x2 && y1 == y2) {
		DrawPixel(x1, y1, c);
	}
	else if (x1 == x2) {
		int inc = (y1 <= y2) ? 1 : -1;
		for (y = y1; y != y2; y += inc) DrawPixel(x1, y, c);
		DrawPixel(x2, y2, c);
	}
	else if (y1 == y2) {
		int inc = (x1 <= x2) ? 1 : -1;
		for (x = x1; x != x2; x += inc) DrawPixel(x, y1, c);
		DrawPixel(x2, y2, c);
	}
	else {
		int dx = (x1 < x2) ? x2 - x1 : x1 - x2;
		int dy = (y1 < y2) ? y2 - y1 : y1 - y2;
		if (dx >= dy) {
			if (x2 < x1) x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
			for (x = x1, y = y1; x <= x2; x++) {
				DrawPixel(x, y, c);
				rem += dy;
				if (rem >= dx) {
					rem -= dx;
					y += (y2 >= y1) ? 1 : -1;
					DrawPixel(x, y, c);
				}
			}
			DrawPixel(x2, y2, c);
		}
		else {
			if (y2 < y1) x = x1, y = y1, x1 = x2, y1 = y2, x2 = x, y2 = y;
			for (x = x1, y = y1; y <= y2; y++) {
				DrawPixel(x, y, c);
				rem += dx;
				if (rem >= dy) {
					rem -= dy;
					x += (x2 >= x1) ? 1 : -1;
					DrawPixel(x, y, c);
				}
			}
			DrawPixel(x2, y2, c);
		}
	}
}