#pragma once

#include <math.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
#define EPSILON			0.001f
#define TRACEDEPTH		6
#define PI				3.1415f

	int CMID(int x, int min, int max) { return (x < min) ? min : ((x > max) ? max : x); }
	float interp(float x1, float x2, float t) { return x1 + (x2 - x1) * t; }

	class Vec
	{
	public:
		float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;
		Vec() = default;
		Vec(float _x, float _y, float _z, float _w = 0.0f):x(_x), y(_y), z(_z), w(_w) {}
		
		friend float dot(const Vec &x, const Vec &y) { return x.x*y.x + x.y*y.y + x.z*y.z; }
		void Debug() { cout << "vec: " << x << " " << y << " " << z << " _ "  << w << endl; }
		friend Vec cross(const Vec &x, const Vec& y) {
			float m1, m2, m3;
			Vec z;
			m1 = x.y * y.z - x.z * y.y;
			m2 = x.z * y.x - x.x * y.z;
			m3 = x.x * y.y - x.y * y.x;
			z.x = m1;
			z.y = m2;
			z.z = m3;
			z.w = 1.0f;
			return z;
		}

		friend Vec operator + (const Vec & v1, const Vec& v2) { return Vec(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }
		friend Vec operator - (const Vec & v1, const Vec& v2) { return Vec(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }
		Vec operator* (float k) { return Vec(this->x*k, this->y*k, this->z*k, this->w*k); }

		float Len() { return sqrt(x*x + y*y + z*z); }
		void Nomalize() {
			float inv_lenth = 1.0f / Len();
			x = x * inv_lenth;
			y = y * inv_lenth;
			z = z * inv_lenth;
		}
		friend Vec division(Vec a, Vec b, float width) {
			return Vec((b.x - a.x) / width, (b.y - a.y) / width, (b.z - a.z) / width);
		}
	};

	Vec interp(const Vec& v1, const Vec& v2, float t) { return Vec(interp(v1.x, v2.x, t), interp(v1.y, v2.y, t), interp(v1.z, v2.z, t)); }

	// 计算a, b, c三个点，按照右手的方式，旋转产生的单位法向量
	Vec calculate_normal(Vec a, Vec b, Vec c) {
		Vec a_b = b - a;
		Vec a_c = c - a;
		Vec ret = cross(a_b, a_c);
		ret.Nomalize();
		return ret;
	}

	class Mat {
	public:
		Mat() {
			for (int i = 0; i < 4; i++) 
				for (int j = 0; j < 4; j++) 
					m[i][j] = 0.0f;
		}
		float m[4][4];
		friend Mat operator + (const Mat & n1, const Mat & n2) {
			Mat ret;
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					ret.m[i][j] = n1.m[i][j] + n2.m[i][j];
				}
			}
			return ret;
		}
		void Debug() {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					cout << m[i][j] << " ";
				} cout << endl;
			}
		}

		friend Mat operator - (const Mat & n1, const Mat & n2) {
			Mat ret;
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					ret.m[i][j] = n1.m[i][j] - n2.m[i][j];
				}
			}
			return ret;
		}

		friend Mat operator * (const Mat & n1, const Mat & n2) {
			Mat ret;
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					ret.m[i][j] = 0.0f;
					for (int k = 0; k < 4; k++) {
						ret.m[i][j] += (n1.m[i][k] * n2.m[k][j]);
					}
				}
			}
			return ret;
		}
		
		void Scale(float x) {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					m[i][j] *= x;
				}
			}
		}
		
		void Set_zero() {
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					m[i][j] = 0.0f;
		}

		void Set_identity() {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++)
					m[i][j] = 0.0f;
				m[i][i] = 1.0f;
			}
		}


		Vec Left_multi(Vec rhs) {
			float x = m[0][0] * rhs.x + m[0][1] * rhs.y + m[0][2] * rhs.z + m[0][3] * rhs.w;
			float y = m[1][0] * rhs.x + m[1][1] * rhs.y + m[1][2] * rhs.z + m[1][3] * rhs.w;
			float z = m[2][0] * rhs.x + m[2][1] * rhs.y + m[2][2] * rhs.z + m[2][3] * rhs.w;
			float w = m[3][0] * rhs.x + m[3][1] * rhs.y + m[3][2] * rhs.z + m[3][3] * rhs.w;
			return Vec(x, y, z, w);
		}


	};


	Mat get_camera_matrix(Vec e, Vec a, Vec u) {

		Mat ret;
		Vec zaix = a - e;
		zaix.Nomalize();
		Vec xaix = cross(u, zaix);
		xaix.Nomalize();
		Vec yaix = cross(zaix, xaix);
		yaix.Nomalize();

	
		ret.m[0][0] = xaix.x;
		ret.m[0][1] = xaix.y;
		ret.m[0][2] = xaix.z;
		ret.m[0][3] = -dot(xaix, e);

		ret.m[1][0] = yaix.x;
		ret.m[1][1] = yaix.y;
		ret.m[1][2] = yaix.z;
		ret.m[1][3] = -dot(yaix, e);

		ret.m[2][0] = zaix.x;
		ret.m[2][1] = zaix.y;
		ret.m[2][2] = zaix.z;
		ret.m[2][3] = -dot(zaix, e);

		ret.m[3][0] = ret.m[3][1] = ret.m[3][2] = 0.0f;
		ret.m[3][3] = 1.0f;

		return ret;

	}

	Mat get_perspective_matrix(float fov, float asp, float zn, float zf) {
		Mat ret;
		float fax = 1.0f / (float)tan(fov * 0.5f);
		ret.Set_zero();
		ret.m[0][0] = (float)(fax / asp);
		ret.m[1][1] = (float)(fax);
		ret.m[2][2] = zf / (zf - zn);
		ret.m[2][3] = -zn * zf / (zf - zn);
		ret.m[3][2] = 1;
		return ret;
	}

	Mat transpose(const Mat & rhs) {
		Mat ret;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				ret.m[i][j] = rhs.m[j][i];
			}
		}
		return ret;
	}

	Mat get_rotate_maxtix(Vec v, float theta) {
		Mat ret;
		float qsin = float(sin(theta * 0.5f));
		float qcos = float(cos(theta * 0.5f));
		Vec tmp(v.x, v.y, v.z, 1.0f);
		float w = qcos;
		tmp.Nomalize();
		float x = tmp.x * qsin;
		float y = tmp.y * qsin;
		float z = tmp.z * qsin;
		ret.m[0][0] = 1 - 2 * y * y - 2 * z * z;
		ret.m[0][1] = 2 * x * y - 2 * w * z;
		ret.m[0][2] = 2 * x * z + 2 * w * y;
		ret.m[1][0] = 2 * x * y + 2 * w * z;
		ret.m[1][1] = 1 - 2 * x * x - 2 * z * z;
		ret.m[1][2] = 2 * y * z - 2 * w * x;
		ret.m[2][0] = 2 * x * z - 2 * w * y;
		ret.m[2][1] = 2 * y * z + 2 * w * x;
		ret.m[2][2] = 1 - 2 * x * x - 2 * y * y;
		ret.m[3][0] = ret.m[3][1] = ret.m[3][2] = 0.0f;
		ret.m[0][3] = ret.m[1][3] = ret.m[2][3] = 0.0f;
		ret.m[3][3] = 1.0f;
		return ret;
	}

	////下面是一些在世界坐标系中变化的矩阵
	Mat get_tarnslate_matrix(float x, float y, float z) {
		Mat ret;
		ret.Set_identity();
		ret.m[0][3] = x;
		ret.m[1][3] = y;
		ret.m[2][3] = z;
		return ret;
	}

	Mat get_scale_matrix(float x, float y, float z) {
		Mat ret;
		ret.Set_identity();
		ret.m[0][0] = x;
		ret.m[1][1] = y;
		ret.m[2][2] = z;
		return ret;
	}


	