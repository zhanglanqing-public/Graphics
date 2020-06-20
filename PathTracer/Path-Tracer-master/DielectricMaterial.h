#pragma once
#include "Material.h"
#include "rgb.h"
#include "Vector3.h"

class Texture;

class DielectricMaterial : public Material
{
public:
	DielectricMaterial() { 
		this->color = rgb(0.999, 0.999, 0.999); 
	}

	virtual void setColor(const rgb& x) override {
		this->color = x;
	}

	virtual rgb AmbientResponse(const Vector3 &tex_point, const Vector2 &uv) override {
		return this->color;
	}


	virtual bool SpecularDirection(const ONB &uvw,
		const Vector3 &in_dir,
		const Vector3 &texp,
		const Vector2 &uv,
		Vector2 &rseed,
		rgb &ratio,
		Vector3 &reflection) override
	{
		Vector3 normal = uvw.w;
		float cosine = Dot(in_dir, normal);
		if (cosine < 0.0f)
			reflection = Reflect(in_dir, normal);
		else
			reflection = Reflect(in_dir, -normal);
		return true;
	}

	virtual bool IsDielectric() override { return true; }




	rgb color;

};

