#pragma once

#include "Screen.h"
#include "Transformer.h"
#include "Scanline.h"
#include <vector>
#include <iostream>
#include <map>
#include "Texture.h"


// ��Ⱦһ���������������(model����任֮���)ʹ�õ�
// �ṩ�����������Ϣ��һ��ͼƬ����Ⱦ��ѡ�������Ⱦ��ͼ����
// ���opengl��״̬��˼��

class Render {

public:
	
	Screen *screen;
	int window_x_size, window_y_size;
	int render_state;
	Color background;
	float* zbuffer = new float[601 * 801];// ��Ȼ��棬ÿ������
	Tranformer tran;
	Color wire;
	Image* diff_im;
	Image* spec_im;
	map<string, Image*> hashing;

	// ��Ⱦ�����������
	Vec light_pos = Vec(5.0f, 5.0f, 5.0f, 1.0f);
	Color light_col = Color(1.0f, 1.0f, 1.0f);
	float ambientStrength = 0.2f;
	float specularStrength = 1.0f;

	// ��ʱ��¼һ���۾���λ�ã����ڹ���ģ�͵ļ���
	Vec eye_position;
	// ��ʱ��¼��ǰ��Ⱦ�����εĵķ�������С
	Vec normal;

	Render(int _w = 800, int _h = 600) {
		// ��Ⱦ�豸��¼buffer��Ҫʹ�ô��ڵĴ�С
		window_x_size = _w;
		window_y_size = _h;
		// �任������Ҫʹ�ô��ڵĴ�С
		tran.wid = _w;
		tran.hgt = _h;
		
		// �ײ����Ļ��Ҫʹ�ô��ڵĴ�С
		screen = new Screen();
		screen->Init(_w, _h);
		
		// �߿�ģʽ��ʹ�õ��߿����ɫ
		wire = Color(1.0, 1.0, 1.0);
		
		// ������ɫ
		background = Color(0.0, 0.0, 0.0);
		
		for (int i = 0; i < window_y_size; i++) {
			for (int j = 0; j < window_x_size; j++) {
				int offset = i * window_y_size + j;
				zbuffer[offset] = 0.0f;
			}
		}
	}

	void Change_diffuse_picture(string filename) { 
		if (hashing.find(filename) != hashing.end()) {
			diff_im = hashing[filename];
		} else {
			diff_im = readPPM().read(filename.c_str());
			hashing[filename] = diff_im;
		}
	}

	void Change_specular_picture(string filename) {
		if (hashing.find(filename) != hashing.end()) {
			spec_im = hashing[filename];
		} else {
			spec_im = readPPM().read(filename.c_str());
			hashing[filename] = spec_im;
		}
	}

	void Change_state(int option) { render_state = option; }
	
	// �ı��ӽ�
	void LookAt(Vec e, Vec a, Vec u) {
		eye_position = e;
		tran.view = get_camera_matrix(e, a, u);
		tran.Update();
	}

	void SetWorldPositionAndState(Mat x) {
		tran.world = x;
	}



	void Draw_scanline(ScanLine &scanline) {
		int x = scanline.x;
		int w = scanline.w;
		
		int width = window_x_size;
		float *cur = zbuffer + scanline.y * window_y_size;
		
		for (; w > 0; x++, w--) {
			if (x >= 0 && x < width) {
				
				float rhw = scanline.v.in_w;
				if (rhw >= cur[x]) {
					float w = 1.0f / rhw;
					cur[x] = rhw;
					
					if (render_state == RENDER_STATE_TEXTURE) {

						float u = scanline.v.tex.u * w;
						float v = scanline.v.tex.v * w;
						Color original_diffuse = Get_diff_texture(u, v);

#ifdef OPENLIGHTMODEL //---�ر�/����---����ģ��---
						Color original_sepcular = Get_spec_texture(u, v);

						// �������յ�ǿ��
						Color ambient = original_diffuse * ambientStrength;

						// �����������ǿ��
						Vec light_dir = light_pos - scanline.cur_pos;
						light_dir.Nomalize();
						float diff = max(dot(normal, light_dir), 0.0f);
						Color diffuse_color = original_diffuse * diff;


						// �߹��ǿ��
						Color specular_color;
						Vec d_view_dir = eye_position - scanline.cur_pos;
						d_view_dir.Nomalize();
						
						light_dir = scanline.cur_pos - light_pos;
						light_dir.Nomalize();

						float tmp = dot(light_dir, normal);
						if (tmp > 0) {
							specular_color = Color(0.0f, 0.0f, 0.0f);
						} else {
							
							Vec reflect_dir = light_dir - normal * (2 * tmp);
							reflect_dir.Nomalize();
							float spec = pow(max(dot(d_view_dir, reflect_dir), 0.0), 1); // bug? 32 doesn't work well
							specular_color = original_sepcular * specularStrength * spec;
							
						}

						Color final_color = ambient + specular_color + diffuse_color;

						screen->DrawPixel(x, scanline.y, final_color.ToUINT());
#else 
						screen->DrawPixel(x, scanline.y, original_diffuse.ToUINT());
#endif


					}
				}
			}
			scanline.v = scanline.v + scanline.step;
			scanline.cur_pos = scanline.cur_pos + scanline.pos_step;
			if (x >= width) break;
		}
	}

	void Draw_Standard(StandardTriangle tri) {
		ScanLine scanline;
		int j, top, bottom;
		bottom = (int)(tri.down + 0.5f);
		top = (int)(tri.top + 0.5f);
		
		for (j = bottom; j < top; j++) {
			if (j >= 0 && j < window_y_size) {
				standardTriangle_edge_interp(tri, (float)j + 0.5f);
				scanline = get_scan_line(tri, j);
				Draw_scanline(scanline);
			}
			if (j >= window_y_size) break;
		}
	}

	// ����Ϊ��һ������Ⱦ�ĺ��ģ�Ҳ����Ⱦ����ڳ�����ϸ�Ĺ��̼�������־
	void Draw_primitive_triangle(Vertex v1, Vertex v2, Vertex v3) {
		
		
		auto x = (v1.norm.x + v2.norm.x + v3.norm.x) / 3.0;
		auto y = (v1.norm.y + v2.norm.y + v3.norm.y) / 3.0;
		auto z = (v1.norm.z + v2.norm.z + v3.norm.z) / 3.0;
		normal = Vec(x, y, z);
		normal.Nomalize();
		Vec p1, p2, p3, c1, c2, c3;

		c1 = tran.Apply_transform(v1.point);
		c2 = tran.Apply_transform(v2.point);
		c3 = tran.Apply_transform(v3.point);
		
		// �ü���ע��˴���������Ϊ�����жϼ�������cvv���Լ�ͬcvv�ཻƽ����������
		// ���н�һ����ϸ�ü�����һ���ֽ�Ϊ������ȫ����cvv�ڵ�������

		// Ȼ������ʵ���Ͻ��е��ǣ�ֻҪ��һ���㲻�ٷ�Χ�ڲ����ͷ���
		if (tran.Check_cvv(c1) != 0) return;
		if (tran.Check_cvv(c2) != 0) return;
		if (tran.Check_cvv(c3) != 0) return;


		// ��һ��
		p1 = tran.Get_screen_point(c1);
		p2 = tran.Get_screen_point(c2);
		p3 = tran.Get_screen_point(c3);
		
		if (render_state == RENDER_STATE_TEXTURE) {
			Vertex t1 = v1, t2 = v2, t3 = v3;
			std::vector<StandardTriangle> retv;
			
			t1.point = p1; // ʵ���� point �����õ���Ϣ�Ѿ�ֻ��x��y��
			t2.point = p2; 
			t3.point = p3;
			t1.point.w = c1.w; // �����wʵ���Ͼ���ԭ����z
			t2.point.w = c2.w;
			t3.point.w = c3.w;
			
			t1.Standard_lize_texture_color();	
			t2.Standard_lize_texture_color();	
			t3.Standard_lize_texture_color();	

			// ���������Ϊ0-2������׼�������Ρ������ҷ��ؿ��á���׼�������Ρ�����
			retv = get_standard_triangle(t1, t2, t3, v1.point, v2.point, v3.point);
			for (auto x: retv) Draw_Standard(x);
		}

		if (render_state == RENDER_STATE_WIREFRAME) {		// �߿����
			screen->DrawLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, wire.ToUINT());
			screen->DrawLine((int)p2.x, (int)p2.y, (int)p3.x, (int)p3.y, wire.ToUINT());
			screen->DrawLine((int)p1.x, (int)p1.y, (int)p3.x, (int)p3.y, wire.ToUINT());
		}
	}

	Color Get_diff_texture(float u, float v) {
		
		// TODO: �������겻��0-1��Χ֮����δ���
		if (!(0 <= u && u <= 1 && 0 <= v && v <= 1)) {
			/*u = u + 10.0f;
			v = v + 10.0f;
			u = u - floor(u);
			v = v - floor(v);*/
			u = 0.0f;
			v = 0.0f;
		}
		
		int x, y;
		x = u * (diff_im->width - 1);
		y = v * (diff_im->height - 1);
		return diff_im->pixel[x + y * diff_im->width];
	}
	Color Get_spec_texture(float u, float v) {
		assert(0 <= u && u <= 1 && 0 <= v && v <= 1);
		int x, y;
		x = u * (spec_im->width - 1);
		y = v * (spec_im->height - 1);
		return spec_im->pixel[x + y * spec_im->width];
	}

	void Clear() {
		int y, x;
		IUINT32 cc = Color(0.0f, 0.0f, 0.0f).ToUINT();
		for (y = 0; y < window_y_size; y++) {
			IUINT32 *dst = screen->framebuffer[y];
			for (x = window_x_size; x > 0; dst++, x--) dst[0] = cc;
		}
		float *p = zbuffer;
		int cnt = 0;
		while (cnt < (801) * (601)) {
			cnt++;
			*p = 0.0f;
			++p;
		}
	}

};