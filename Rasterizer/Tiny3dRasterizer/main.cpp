
#include "Screen.h"
#include "Math.h"
#include "Scanline.h"
#include "Render.h"
#include "Camera.h"
#include "Texture.h"
#include "LightModel.h"


#define WID 800
#define HGT 600
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0) 

int main()
{

	TCHAR *title = _T("Tiny_3d_Rasterizer_developed_by_zhanglanqing");
	screen_Init(WID, HGT, title);
	Render* prender = new Render(WID, HGT);
	Camera* pcam = new Camera();
	LightModel box;
	cout << "<uage & control>" << endl;
	cout << "press WASD" << " move your body." << endl;
	cout << "press IJKL" << " move your eye." << endl;
	cout << "press QE" << " change the scale of the box." << endl;
	cout << "press UO" << " rotate the box." << endl;

	while (screen_exit == 0 && screen_keys[VK_ESCAPE] == 0) {
		Vec eye_postion = pcam->Eye_position();
		Vec lookat_position = pcam->Look_at_position();
		eye_postion.w = lookat_position.w = 1.0;
		prender->LookAt(eye_postion, lookat_position, Vec(0, 0, 1, 1));
		screen_dispatch();
		prender->Clear();

		if (KEY_DOWN('W')) { pcam->Move(FORWARD); }
		if (KEY_DOWN('S')) { pcam->Move(BACKWARD); }
		if (KEY_DOWN('A')) { pcam->Move(LEFT); }
		if (KEY_DOWN('D')) { pcam->Move(RIGHT); }

		if (KEY_DOWN('J')) { pcam->Eye_move(LEFT); }
		if (KEY_DOWN('L')) { pcam->Eye_move(RIGHT); }
		if (KEY_DOWN('I')) { pcam->Eye_move(FORWARD); }
		if (KEY_DOWN('K')) { pcam->Eye_move(BACKWARD); }
		
		if (KEY_DOWN('Q')) { box.Upscale(); }
		if (KEY_DOWN('E')) { box.Downscale(); }
		if (KEY_DOWN('U')) { box.Uptheta(); }
		if (KEY_DOWN('O')) { box.Downtheta(); }
		
		box.Draw(prender);

		screen_update();
		Sleep(1);
	}
	return 0;
}
