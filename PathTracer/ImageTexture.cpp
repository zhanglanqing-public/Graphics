#include "Image.h"
#include "ImageTexture.h"

ImageTexture::ImageTexture(char *file_name) : image(new Image())
{
	image->ReadPPM(file_name);
}

rgb ImageTexture::Value(const Vector2 &uv, const Vector3 &pos) const
{
	
	float u = uv.x -int(uv.x);
	float v = uv.y - int(uv.y);
	if (u < 0.0) u = 0.0;
	if (u > 1.0) u = 1.0;
	if (v < 0.0) v = 0.0;
	if (v > 1.0) v = 1.0;

	u *= (image->Width() - 3);
	v *= (image->Height() - 3);

	int iu = (int)u;
	int iv = (int)v;

	float tu = u - iu;
	float tv = v - iv;

	// rgb c = (*image)[iu][iv];
	rgb c = (*image)[iu][iv] * (1 - tu)*(1 - tv) +
		(*image)[iu + 1][iv] * tu*(1 - tv) +
		(*image)[iu][iv + 1] * (1 - tu)*tv +
		(*image)[iu + 1][iv + 1] * tu*tv;
	return c;
}
