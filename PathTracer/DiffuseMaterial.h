#pragma once

#include "Material.h"

class Texture;

class DiffuseMaterial : public Material
{
public:
	
	DiffuseMaterial(Texture *t) : R(t) { this->emission = rgb(); }
	virtual void setEmission(float inintensity) override {
		this->emission = rgb(inintensity); 
	}
	
	virtual rgb AmbientResponse(const Vector3 &tex_point, const Vector2 &uv) override {
		return R->Value(uv, tex_point); 
	}
	virtual bool ExplicitBRDF(const Vector3 &p, const Vector2 &uv, rgb &brdf) override {
		float k = .318309886184f; // 1.0/PI
		brdf = k * this->AmbientResponse(p, uv);
		return true;
	}

	virtual bool DiffuseDirection(const ONB &uvw, 
		const Vector3 &, 
		const Vector3 &p, 
		const Vector2 &uv, 
		Vector2 &seed, 
		rgb &color,
		Vector3 &v_out) override
	{
		float pi = 3.1415926535f;
		float phi = 2 * pi*seed.x;
		float r = sqrt(seed.y);
		float x = r*cos(phi);
		float y = r*sin(phi);
		float z = sqrt(1 - x*x - y*y);

		color = this->AmbientResponse(p, uv);

		v_out = x*uvw.u + y*uvw.v + z*uvw.w;

		seed.Scramble();
		return true;
	}
	
	virtual bool IsDiffuse() override { return true; }
public:
	Texture *R;
};



