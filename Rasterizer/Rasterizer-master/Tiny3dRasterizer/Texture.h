#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <vector>
#include <string>
#include <iostream>
#include <assert.h>

#include "Math.h"
#include "Scanline.h"
#include <string>

class Image {
public:
	Image(int row, int col, int maxer_depth = 255) {
		pixel = new Color[row*col + 1];
		width = col;
		height = row;
		colorMax = maxer_depth;
		sz = 0;
	}
	int height;
	int width;
	int colorMax;
	std::string filename;
	Color* pixel;
	int sz;
	~Image() {
		delete[]pixel;
	}
};


class readPPM
{
public:
	readPPM() {};
	~readPPM() {};

	//�����ascll���� ����true
	bool isAscll(FILE* fp) {
		if (toupper(fgetc(fp)) == 'P'&&fgetc(fp) == '3') {
			return true;
		}
		else {
			return false;
		}
	}

	//ɾ��ע��
	void deleteNotes(FILE* fp) {
		while (fgetc(fp) != '#') {
			continue;
		}
		while (fgetc(fp) != '\n') {
			continue;
		}
	}

	//��ȡ��һ�����ֻ�ɾ���м��ע��
	int  readDigit(FILE* fp) {
		int colorNum;
		while (!fscanf_s(fp, "%d", &colorNum)) {
			deleteNotes(fp);
		}
		return colorNum;
	}

	//��ȡͼƬ �� �� ��ɫ��Χ ������Ϣ
	void getIniInfo(FILE* fp, int* width, int* height, int* colorMax) {
		int* info[3] = { colorMax, width, height };
		int n = 2;
		do {
			*info[n] = readDigit(fp);
		} while (n--);
	}

	

	Image* read(std::string filename) {

		FILE* fp;
		errno_t err;
		err = fopen_s(&fp, filename.c_str(), "r");
		if (err) {
			throw runtime_error("can not find the texturefile, plz check again..");
		}
		if (isAscll(fp)) {
			int w, h, maxer;
			getIniInfo(fp, &w, &h, &maxer);
			Image* img = new Image(w, h, maxer);

			/*printf("width:%d\n", img->width);
			printf("height:%d\n", img->height);
			printf("colorMax:%d\n", img->colorMax);*/

			while (!feof(fp)) {
				Color rgb;
				rgb.r = readDigit(fp) * 1.0 / img->colorMax;
				rgb.g = readDigit(fp) * 1.0 / img->colorMax;
				rgb.b = readDigit(fp) * 1.0 / img->colorMax;
				img->pixel[img->sz++] = rgb;
			}
			fclose(fp);
			return img;
		} else {
			std::cout << "this type of texture file is not supported.\nAnd we only support P3 file instead." << std::endl;
		}
		fclose(fp);
		return NULL;
	}
};