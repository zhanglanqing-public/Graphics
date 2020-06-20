
#pragma once
#include "Math.h"
#include "Screen.h"
#include "Transformer.h"
#include <vector>
class Color {
public:
	float r, g, b;
	Color(float _r, float _g, float _b) : r(_r), g(_g), b(_b) {}
	Color() { r = g = b = 0.0f; }
	IUINT32 ToUINT() {
		int R = (int)(r * 255.0f);
		int G = (int)(g * 255.0f);
		int B = (int)(b * 255.0f);
		R = CMID(R, 0, 255);
		G = CMID(G, 0, 255);
		B = CMID(B, 0, 255);
		return (R << 16) | (G << 8) | (B);
	}
	Color operator * (float x) {
		return Color(r * x, g * x, b * x);
	}
	Color operator + (const Color & oth) {
		return Color(oth.r + r, oth.g + g, oth.b + b);
	}
};

class Texture {
public:
	float u, v;
	Texture(float _u, float _v) :u(_u), v(_v) {}
	Texture() { u = v = 0.0f; }
};


class Vertex {
public:
	Vec point;
	Vec norm; // 这个属性只有在画三角形的时候能够使用到，并不参与线性差值计算
	Texture tex;
	float in_w; // 这个值是point的w的倒数，也就是1/z

	Vertex() {}
	Vertex(Vec _p, Texture _tex, float _inw) : point(_p), tex(_tex), in_w(_inw) { norm = Vec(); }

	// the x, y, z, have been stardard_lize.
	void Standard_lize_texture_color() {
		in_w = 1.0f / float(point.w);
		tex.u *= in_w;
		tex.v *= in_w;
	}

	friend Vertex operator + (Vertex v1, Vertex v2) {
		Vertex ret;
		ret.tex.u = v1.tex.u + v2.tex.u;
		ret.tex.v = v1.tex.v + v2.tex.v;
		ret.point = v1.point + v2.point;
		ret.in_w = v1.in_w + v2.in_w;
		return ret;
	}

	friend Vertex division(Vertex x1, Vertex x2, float w) {
		Vertex y;
		float inv = 1.0f / w;
		y.point.x = (x2.point.x - x1.point.x) * inv;
		y.point.y = (x2.point.y - x1.point.y) * inv;
		y.point.z = (x2.point.z - x1.point.z) * inv;
		y.point.w = (x2.point.w - x1.point.w) * inv;
		y.tex.u = (x2.tex.u - x1.tex.u) * inv;
		y.tex.v = (x2.tex.v - x1.tex.v) * inv;
		y.in_w = (x2.in_w - x1.in_w) * inv;
		return y;
	}
};
// 节点插值
Vertex interp(Vertex v1, Vertex v2, float t) {
	Vec tmp_vec = interp(v1.point, v2.point, t);
	Texture tmp_tex = Texture(interp(v1.tex.u, v2.tex.u, t), interp(v1.tex.v, v2.tex.v, t));
	float tmp_inw = interp(v1.in_w, v2.in_w, t);
	return Vertex(tmp_vec, tmp_tex, tmp_inw);
}


class Edge {
public:
	Edge() = default;
	Edge(Vertex _v1, Vertex _v2, Vec pos1, Vec pos2, Vertex _now = Vertex())  // 传递原来的vXpos是为了光照模型的原来位置的计算
		:v1(_v1), v2(_v2), v1pos(pos1), v2pos(pos2), now(_now) {}
	Vertex v1, v2, now;
	Vec v1pos, v2pos, pos_now; // 边的两端的原来的position位置
};

class StandardTriangle {
public:
	StandardTriangle() = default;
	StandardTriangle(Edge _left, Edge _right, float _top, float _down) 
		:left(_left), right(_right), top(_top), down(_down) {}
	float top, down;
	Edge left, right;

};

// 根据三角形生成 0-2 个“标准化三角形”，并且返回合法“标准化三角形”的数量，这里的三角形是实际上要绘制的二维三角形
std::vector<StandardTriangle> get_standard_triangle(Vertex v1, Vertex v2, Vertex v3, Vec v1pos, Vec v2pos, Vec v3pos) { // 这里传递原来的坐标主要是为了计算光照模型
	std::vector<StandardTriangle> ret;
	// 将p1, p2, p2按照纵坐标的大小从下往上进行排布
	if (v1.point.y > v2.point.y) std::swap(v1, v2);
	if (v1.point.y > v3.point.y) std::swap(v1, v3);
	if (v2.point.y > v3.point.y) std::swap(v2, v3);

	//三角形退化成了直线
	if (v1.point.y == v2.point.y && v1.point.y == v3.point.y) return ret;
	if (v1.point.x == v2.point.x && v1.point.x == v3.point.x) return ret;

	// 如果形成的是下三角形，调整v1，v2的位置，v1在左边。
	if (v1.point.y == v2.point.y) {
		if (v1.point.x > v2.point.x) std::swap(v1, v2);
		StandardTriangle s;
		s.top = v3.point.y;
		s.down = v1.point.y;
		s.left = Edge(v1, v3, v1pos, v3pos);
		s.right = Edge(v2, v3, v2pos, v3pos);

		ret.push_back(s);
		return ret;
	}

	// 如果形成的是上三角形，调整v2，v3的位置，v2在左边。
	if (v2.point.y == v3.point.y) {
		if (v2.point.x > v3.point.x) std::swap(v2, v3);
		StandardTriangle s;
		s.top = v3.point.y;
		s.down = v1.point.y;
		s.left = Edge(v1, v2, v1pos, v2pos);
		s.right = Edge(v1, v3, v1pos, v3pos);
		ret.push_back(s);
		return ret;
	}

	// 其他情况返回两个三角形，是上下分割之后形成的。
	StandardTriangle s, t;

	s.top = v2.point.y;
	s.down = v1.point.y;
	t.top = v3.point.y;
	t.down = v2.point.y;


	// 判断这个三角形的凸出来的那部分是在左边还是在右边。
	float k = (v3.point.y - v1.point.y) / (v2.point.y - v1.point.y);
	float x = v1.point.x + k * (v2.point.x - v1.point.x);
	if (x <= v3.point.x) { // 三角形是在左边的类型
		// s代表的是下边的
		s.left = Edge(v1, v2, v1pos, v2pos);
		s.right = Edge(v1, v3, v1pos, v3pos);
		// t代表的是上边的
		t.left = Edge(v2, v3, v2pos, v3pos);
		t.right = Edge(v1, v3, v1pos, v3pos);
	} else { // 三角形是右边的类型
		s.left = Edge(v1, v3, v1pos, v3pos);
		s.right = Edge(v1, v2, v1pos, v2pos);
		t.left = Edge(v1, v3, v1pos, v3pos);
		t.right = Edge(v2, v3, v2pos, v3pos);
	}

	ret.push_back(s);
	ret.push_back(t);
	return ret;

}

// 按照 Y 坐标计算出左右两条边纵坐标等于 Y 的顶点，存放在三角形的两边的两条边上“正在处理的点”这个分量里面
void standardTriangle_edge_interp(StandardTriangle& tri, float y) {
	float s1 = tri.left.v2.point.y - tri.left.v1.point.y;
	float s2 = tri.right.v2.point.y - tri.right.v1.point.y;
	float t1 = (y - tri.left.v1.point.y) / s1;
	float t2 = (y - tri.right.v1.point.y) / s2;
	tri.left.now = interp(tri.left.v1, tri.left.v2, t1);
	tri.right.now = interp(tri.right.v1, tri.right.v2, t2);
	tri.left.pos_now = interp(tri.left.v1pos, tri.left.v2pos, t1);
	tri.right.pos_now = interp(tri.right.v1pos, tri.right.v2pos, t2);
}



class ScanLine {
public:
	ScanLine() = default;
	
	Vertex v; //这个扫描线当前正在处理的点
	Vertex step;
	int x, y, w; // 当前扫描线的相关信息，扫描线的横坐标的开始x和结束w，以及当前扫描的y值。
	
	// 仿照上述的方法，同时记录-开始-到-结束的-原来坐标的位置
	Vec x_pos, w_pos;
	Vec pos_step;
	Vec cur_pos;
};

// 根据左右两边的端点，初始化计算出扫描线的起点和步长，实际上这个扫描线就是x轴方向的，step也就是只有一个方向有分量
ScanLine get_scan_line(const StandardTriangle &tri, int y) {
	float width = tri.right.now.point.x - tri.left.now.point.x;
	ScanLine ret;

	ret.y = y;
	
	ret.x = (int)(tri.left.now.point.x + 0.5f);
	ret.w = (int)(tri.right.now.point.x + 0.5f) - ret.x;
	ret.v = tri.left.now;
	ret.step = division(tri.left.now, tri.right.now, width);

	if (tri.left.now.point.x >= tri.right.now.point.x) ret.w = 0;
	
	
	// 设置开始的点的原来的位置
	ret.x_pos = tri.left.pos_now;
	ret.w_pos = tri.right.pos_now;
	ret.cur_pos = ret.x_pos;
	ret.pos_step = division(ret.x_pos, ret.w_pos, width);

	/*std::cout << "now == test" << y << std::endl;
	std::cout << "test ---- " << endl << ret.x_pos.x << " " << ret.x_pos.y << " " << ret.x_pos.z << std::endl;
	std::cout << "test ---- " << endl << ret.w_pos.x << " " << ret.w_pos.y << " " << ret.w_pos.z << std::endl;
	system("pause");*/

	return ret;
}


