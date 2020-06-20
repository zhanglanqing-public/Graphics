#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <windows.h>
#include <tchar.h>

#define RENDER_STATE_WIREFRAME      1		// ��Ⱦ�߿�
#define RENDER_STATE_TEXTURE        2		// ��Ⱦ����

// #define OPENLIGHTMODEL

typedef unsigned int IUINT32;

//=====================================================================
// Win32 ���ڼ�ͼ�λ��ƣ�Ϊ device �ṩһ�� DibSection �� FB
//=====================================================================
extern int screen_w, screen_h, screen_exit;
extern int screen_mx, screen_my, screen_mb;
extern int screen_keys[512];	// ��ǰ���̰���״̬
extern HWND screen_handle;		// ������ HWND
extern HDC screen_dc;			// ���׵� HDC
extern HBITMAP screen_hb;		// DIB
extern HBITMAP screen_ob;		// �ϵ� BITMAP
extern unsigned char *screen_fb;// frame buffer
extern long screen_pitch;

extern int screen_Init(int w, int h, const TCHAR *title);	// ��Ļ��ʼ��
extern int screen_close(void);								// �ر���Ļ
extern void screen_dispatch(void);							// ������Ϣ
extern void screen_update(void);							// ��ʾ FrameBuffer

// win32 event handler
extern  LRESULT screen_events(HWND, UINT, WPARAM, LPARAM);

