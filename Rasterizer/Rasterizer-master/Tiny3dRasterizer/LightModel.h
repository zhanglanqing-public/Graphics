#pragma once
#include "Scanline.h"
#include "Render.h"
#include "Texture.h"


float vertices[] = {
	// Positions          // Normals           // Texture Coords
	0.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	1.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	1.0f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	1.0f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	0.0f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
	0.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

	0.0f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	0.0f, 1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
	1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	0.0f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	//-------------------------------------------------------
	0.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	0.0f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	0.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	0.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	0.0f, 1.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	1.0f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	1.0f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	1.0f, 0.0f,  1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	//-------------------------------------------------------
	0.0f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
	0.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
	1.0f, 0.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	1.0f, 0.0f,  1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	1.0f, 0.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	0.0f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	0.0f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	1.0f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	0.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	0.0f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};


struct LightModel {

	//�������ģ�������е�������ʹ�õ���ͬһ��ͼƬ

	vector<Vertex> mesh;
	
	// ����ϵ������ת�Ƕ�Ӧ������������ԡ�
	float scale = 1.0;
	void Upscale() { scale += 0.1; }
	void Downscale() { scale -= 0.1; }
	float Getscale() { return scale; }

	Vec rotateVec = Vec(0.f, 0.0f, 1.0f, 1.0f);
	float theta = 0.5;
	void Uptheta() { theta += 0.1; }
	void Downtheta() { theta -= 0.1; }
	float Gettheta() { return theta; }



	LightModel() {
		// ʹ�����ݹ������ԵĽӿڣ�vertex�ļ��ϣ�Ҳ����mesh
		
		float* base = vertices;
		for (int i = 0; i < 36; i++) {
			auto position = Vec(base[0] , base[1] , base[2] , 1);
			auto normal = Vec(base[3], base[4], base[5], 1);
			auto texure = Texture(base[6], base[7]);
			auto tmp = Vertex(position, texure, 1);
			tmp.norm = normal;
			mesh.push_back(tmp);
			base += 8;
		}
		// std::cout << "loading model ok " << mesh.size() << endl;
	}
	
	void Move_rotate_scale(Mat model) {
		for (auto& vertex : this->mesh) {
			vertex.point = model.Left_multi(vertex.point);
			vertex.point.w = 1.0;
		}
	}
	
	void Draw(Render* render) {
		render->Change_diffuse_picture("box_diffuse.ppm");
		render->Change_specular_picture("box_spec.ppm");
		// ����Ӧ��ע��˳�����ִ�е�һ����ƽ�Ʊ任����������ƽ�ƵĻ���һ���Ƿ�������ߵ�
		render->SetWorldPositionAndState(get_scale_matrix(scale, scale, scale) * get_rotate_maxtix(rotateVec, theta));
		render->Change_state(RENDER_STATE_TEXTURE);
		for (int i = 0; i < 12; i++) {
			int a = i * 3, b = i * 3 + 1, c = i * 3 + 2;
			render->Draw_primitive_triangle(mesh[a], mesh[b], mesh[c]);
		}
	}

	


	
};

