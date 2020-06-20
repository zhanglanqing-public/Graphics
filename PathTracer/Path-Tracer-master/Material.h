#pragma once

#include "Vector2.h"
#include "Shape.h"
#include "Ray.h"
#include "rgb.h"
#include "ONB.h"

class Material
{
public:

	Material() { this->emission = rgb(); }

	virtual rgb AmbientResponse(const Vector3 &tex_point, const Vector2 &uv) { return rgb(); }
	virtual rgb EmissionResponse() { return this->emission; }


	//////////////////////////////////////////////////////////////////////////
	virtual bool IsDiffuse() { return false; }
	virtual bool ExplicitBRDF(const Vector3 &tex_point, const Vector2 &uv, rgb &) { return false; }
	virtual void setEmission(float inintensity) {}
	virtual bool DiffuseDirection(const ONB &,
		const Vector3 &incident, 
		const Vector3 &tex_point, 
		const Vector2 &uv, 
		Vector2 &seed, 
		rgb &, 
		Vector3 &) {
		return false; 
	}
	


	//////////////////////////////////////////////////////////////////////////
	virtual bool IsPhongMetal() { return false; }
	virtual bool PhongMetalDirection(const ONB &,
		const Vector3 &incident,
		const Vector3 &tex_point,
		const Vector2 &uv,
		Vector2 &seed,
		rgb &,
		Vector3 &) {
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	virtual bool IsDielectric() { return false; }
	virtual bool SpecularDirection(const ONB &uvw,
		const Vector3 &in_dir,
		const Vector3 &tex_pos,
		const Vector2 &uv,
		Vector2 &rseed,
		rgb &ratio,
		Vector3 &reflection) {
		return false;
	}
	virtual void setColor(const rgb&) {}

	//////////////////////////////////////////////////////////////////////////
	virtual bool IsRefract() { return false; }

	virtual bool RefractDirection(const ONB &uvw,
		const Vector3 &in_dir,
		const Vector3 &tex_pos,
		const Vector2 &uv,
		Vector2 &rseed,
		rgb &ratio,
		Vector3 &refraction) {
		return false;
	}

	virtual bool IsMixRefractflec()  { return true; }

	virtual bool RefractflecDirection(const ONB &uvw,
		const Vector3 &in_dir,
		const Vector3 &tex_pos,
		const Vector2 &uv,
		Vector2 &rseed,
		rgb &ratio,
		Vector3 &refraction, Vector3& reflection, float& Fe) {
		return false;
	}


	rgb emission;
};
