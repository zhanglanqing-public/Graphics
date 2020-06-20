#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <windows.h>
#include <tchar.h>

#define RENDER_STATE_WIREFRAME      1		// 渲染线框
#define RENDER_STATE_TEXTURE        2		// 渲染纹理

// #define OPENLIGHTMODEL

typedef unsigned int IUINT32;

//=====================================================================
// Win32 窗口及图形绘制：为 device 提供一个 DibSection 的 FB
//=====================================================================
extern int screen_w, screen_h, screen_exit;
extern int screen_mx, screen_my, screen_mb;
extern int screen_keys[512];	// 当前键盘按下状态
extern HWND screen_handle;		// 主窗口 HWND
extern HDC screen_dc;			// 配套的 HDC
extern HBITMAP screen_hb;		// DIB
extern HBITMAP screen_ob;		// 老的 BITMAP
extern unsigned char *screen_fb;// frame buffer
extern long screen_pitch;

extern int screen_Init(int w, int h, const TCHAR *title);	// 屏幕初始化
extern int screen_close(void);								// 关闭屏幕
extern void screen_dispatch(void);							// 处理消息
extern void screen_update(void);							// 显示 FrameBuffer

// win32 event handler
extern  LRESULT screen_events(HWND, UINT, WPARAM, LPARAM);

