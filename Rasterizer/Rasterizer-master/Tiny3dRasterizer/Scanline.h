
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
	Vec norm; // �������ֻ���ڻ������ε�ʱ���ܹ�ʹ�õ��������������Բ�ֵ����
	Texture tex;
	float in_w; // ���ֵ��point��w�ĵ�����Ҳ����1/z

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
// �ڵ��ֵ
Vertex interp(Vertex v1, Vertex v2, float t) {
	Vec tmp_vec = interp(v1.point, v2.point, t);
	Texture tmp_tex = Texture(interp(v1.tex.u, v2.tex.u, t), interp(v1.tex.v, v2.tex.v, t));
	float tmp_inw = interp(v1.in_w, v2.in_w, t);
	return Vertex(tmp_vec, tmp_tex, tmp_inw);
}


class Edge {
public:
	Edge() = default;
	Edge(Vertex _v1, Vertex _v2, Vec pos1, Vec pos2, Vertex _now = Vertex())  // ����ԭ����vXpos��Ϊ�˹���ģ�͵�ԭ��λ�õļ���
		:v1(_v1), v2(_v2), v1pos(pos1), v2pos(pos2), now(_now) {}
	Vertex v1, v2, now;
	Vec v1pos, v2pos, pos_now; // �ߵ����˵�ԭ����positionλ��
};

class StandardTriangle {
public:
	StandardTriangle() = default;
	StandardTriangle(Edge _left, Edge _right, float _top, float _down) 
		:left(_left), right(_right), top(_top), down(_down) {}
	float top, down;
	Edge left, right;

};

// �������������� 0-2 ������׼�������Ρ������ҷ��غϷ�����׼�������Ρ����������������������ʵ����Ҫ���ƵĶ�ά������
std::vector<StandardTriangle> get_standard_triangle(Vertex v1, Vertex v2, Vertex v3, Vec v1pos, Vec v2pos, Vec v3pos) { // ���ﴫ��ԭ����������Ҫ��Ϊ�˼������ģ��
	std::vector<StandardTriangle> ret;
	// ��p1, p2, p2����������Ĵ�С�������Ͻ����Ų�
	if (v1.point.y > v2.point.y) std::swap(v1, v2);
	if (v1.point.y > v3.point.y) std::swap(v1, v3);
	if (v2.point.y > v3.point.y) std::swap(v2, v3);

	//�������˻�����ֱ��
	if (v1.point.y == v2.point.y && v1.point.y == v3.point.y) return ret;
	if (v1.point.x == v2.point.x && v1.point.x == v3.point.x) return ret;

	// ����γɵ����������Σ�����v1��v2��λ�ã�v1����ߡ�
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

	// ����γɵ����������Σ�����v2��v3��λ�ã�v2����ߡ�
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

	// ��������������������Σ������·ָ�֮���γɵġ�
	StandardTriangle s, t;

	s.top = v2.point.y;
	s.down = v1.point.y;
	t.top = v3.point.y;
	t.down = v2.point.y;


	// �ж���������ε�͹�������ǲ���������߻������ұߡ�
	float k = (v3.point.y - v1.point.y) / (v2.point.y - v1.point.y);
	float x = v1.point.x + k * (v2.point.x - v1.point.x);
	if (x <= v3.point.x) { // ������������ߵ�����
		// s��������±ߵ�
		s.left = Edge(v1, v2, v1pos, v2pos);
		s.right = Edge(v1, v3, v1pos, v3pos);
		// t��������ϱߵ�
		t.left = Edge(v2, v3, v2pos, v3pos);
		t.right = Edge(v1, v3, v1pos, v3pos);
	} else { // ���������ұߵ�����
		s.left = Edge(v1, v3, v1pos, v3pos);
		s.right = Edge(v1, v2, v1pos, v2pos);
		t.left = Edge(v1, v3, v1pos, v3pos);
		t.right = Edge(v2, v3, v2pos, v3pos);
	}

	ret.push_back(s);
	ret.push_back(t);
	return ret;

}

// ���� Y ��������������������������� Y �Ķ��㣬����������ε����ߵ��������ϡ����ڴ���ĵ㡱�����������
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
	
	Vertex v; //���ɨ���ߵ�ǰ���ڴ���ĵ�
	Vertex step;
	int x, y, w; // ��ǰɨ���ߵ������Ϣ��ɨ���ߵĺ�����Ŀ�ʼx�ͽ���w���Լ���ǰɨ���yֵ��
	
	// ���������ķ�����ͬʱ��¼-��ʼ-��-������-ԭ�������λ��
	Vec x_pos, w_pos;
	Vec pos_step;
	Vec cur_pos;
};

// �����������ߵĶ˵㣬��ʼ�������ɨ���ߵ����Ͳ�����ʵ�������ɨ���߾���x�᷽��ģ�stepҲ����ֻ��һ�������з���
ScanLine get_scan_line(const StandardTriangle &tri, int y) {
	float width = tri.right.now.point.x - tri.left.now.point.x;
	ScanLine ret;

	ret.y = y;
	
	ret.x = (int)(tri.left.now.point.x + 0.5f);
	ret.w = (int)(tri.right.now.point.x + 0.5f) - ret.x;
	ret.v = tri.left.now;
	ret.step = division(tri.left.now, tri.right.now, width);

	if (tri.left.now.point.x >= tri.right.now.point.x) ret.w = 0;
	
	
	// ���ÿ�ʼ�ĵ��ԭ����λ��
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


