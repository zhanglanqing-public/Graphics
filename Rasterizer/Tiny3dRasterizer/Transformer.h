#pragma once
#include "Math.h"
using namespace std;
class Tranformer {
public:
	
	Mat world;
	Mat view;
	Mat proj;
	Mat trans;
	float wid, hgt;

	Tranformer(int w = 800, int h = 600) {
		world.Set_identity();
		wid = float(w), hgt = float(h);
		view = get_camera_matrix(Vec(3, 3, 3, 1), Vec(0, 0, 0, 1), Vec(0, 0, 1, 1));
		proj = get_perspective_matrix(PI * 0.5f, wid / hgt, 1.0f, 500.0f);
		Update();
	}

	void Update() {
		trans = proj * view * world;
	}

	Vec Apply_transform(Vec x) {
		return trans.Left_multi(x);
	}

	bool Check_cvv(Vec v) { // not in CVV -> true
		float w = v.w;
		int checker = 0;
		if (v.z < 0) checker |= 1;
		if (v.z > w) checker |= 2;
		if (v.x < -w) checker |= 4;
		if (v.x > w) checker |= 8;
		if (v.y < -w) checker |= 16;
		if (v.y > w) checker |= 32;
		return checker != 0;
	}

	// 透视除法，齐次化，得到屏幕坐标。
	Vec Get_screen_point(Vec cvv_point) {
		float in_w = 1.0f / cvv_point.w;
		return Vec((cvv_point.x * in_w + 1.0f) * wid * 0.5f,
					(1.0f - cvv_point.y * in_w) * hgt * 0.5f,
					(cvv_point.z * in_w),
					(1.0f));
	}
};


