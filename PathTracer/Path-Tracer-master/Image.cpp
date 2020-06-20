#include "Image.h"
#include <iostream>
#include "svpng.inc"
using namespace std;

Image::Image()
{ }

Image::Image(int width, int height) : width(width), height(height)
{
	raster = new rgb*[width];
	for (int i = 0; i < width; i++)
		raster[i] = new rgb[height];
}

Image::Image(int width, int height, rgb background) : width(width), height(height)
{
	raster = new rgb*[width];
	for (int i = 0; i < width; i++)
	{
		raster[i] = new rgb[height];
		for (int j = 0; j < height; j++)
			raster[i][j] = background;
	}
}

bool Image::Set(int x, int y, const rgb &color)
{
	if (0 > x || x >= width) return false;
	if (0 > y || y >= height) return false;
	raster[x][y] = color;
	return true;
}

void Image::GammaCorrect(float gamma)
{
	rgb temp;
	float power = 1.0 / gamma;
	for(int i = 0; i<width; i++)
		for (int j = 0; j < height; j++)
		{
			temp = raster[i][j];
			raster[i][j] = rgb(pow(temp.r, power), pow(temp.g, power), pow(temp.b, power));
		}
}

void Image::WritePPM(ostream &out)
{
	out << "P6\n";
	out << width << ' ' << height << '\n';
	out << "255\n";

	int i, j;
	unsigned ired, igreen, iblue;
	unsigned char red, green, blue;

	for(i = height-1; i>=0; i--)
		for (j = 0; j < width; j++)
		{
			ired = (unsigned)(256 * raster[j][i].r);
			igreen = (unsigned)(256 * raster[j][i].g);
			iblue = (unsigned)(256 * raster[j][i].b);
			if (ired > 255) ired = 255;
			if (igreen > 255) igreen = 255;
			if (iblue > 255) iblue = 255;
			red = (unsigned char)(ired);
			green = (unsigned char)(igreen);
			blue = (unsigned char)(iblue);
			out.put(red);
			out.put(green);
			out.put(blue);
		}
}

void Image::ReadPPM(string file_name)
{
	ifstream in;
	// if open the file without the binary flag, get() will stop at '25' (end of medium)
	in.open(file_name.c_str(), ios::binary);
	if (!in.is_open())
	{
		cerr << "ERROR -- Counldn't open file \'" << file_name << "\'.\n";
		exit(-1);
	}
	char ch, type;
	int red, green, blue;
	int i, j, cols, rows;
	int num;

	in.get(ch);
	in.get(type);
	in >> cols >> rows >> num;
	
	width = cols;
	height = rows;

	raster = new rgb*[width];
	for (i = 0; i < width; i++)
		raster[i] = new rgb[height];

	in.get(ch);
	for (i = height - 1; i >= 0; i--)
		for (j = 0; j < width; j++)
		{
			in >> red >> green >> blue;			
			 raster[j][i] = rgb(
				(float)(red / 255.0),
				(float)(green / 255.0),
				(float)(blue / 255.0));

		}
	in.close();
}

void Image::WritePNG(ostream &out)
{
	unsigned char *data = new unsigned char[width*height * 3];
	unsigned char *p = data;
	for(int i = height-1; i>=0; i--)
		for (int j = 0; j<width; j++)
		{
			*p++ = fmin(raster[j][i].r, 1.f)*255;
			*p++ = fmin(raster[j][i].g, 1.f)*255;
			*p++ = fmin(raster[j][i].b, 1.f)*255;
		}
	svpng(out, width, height, data, 0);
	delete[] data;
}

