#pragma once
#include "Material.h"
#include "rgb.h"
#include "Vector3.h"

class Texture;

class MixRefractflecMaterial : public Material
{
public:
	MixRefractflecMaterial(double n)  { 
		nt = n; 
		color = rgb(0.999, 0.999, 0.999); 
	}

	virtual void setColor(const rgb& x) override {
		this->color = x;
	}

	virtual rgb AmbientResponse(const Vector3 &tex_point, const Vector2 &uv) override {
		return this->color;
	}

	virtual bool IsMixRefractflec() override { return true; }

	virtual bool RefractflecDirection(const ONB &uvw,
		const Vector3 &in_dir,
		const Vector3 &tex_pos,
		const Vector2 &uv,
		Vector2 &rseed,
		rgb &ratio,
		Vector3 &refraction, Vector3& reflection, float& Fe) override {
		

		Vector3 normal = uvw.w;

		float cosine = Dot(in_dir, normal);
		double nc = 1.0;
		double nt = this->nt; 
		double nnt;
		bool into = true;
		if (cosine < 0.0) {  // into
			nnt = nc / nt;
			reflection = Reflect(in_dir, normal);
		}
		else {             // out
			into = false;
			reflection = Reflect(in_dir, -normal);
			nnt = nt / nc;
		}
		double ddn = Dot(in_dir, normal) * -1.0;
		double sin_2_t = (1 - ddn * ddn) * nnt * nnt;

		if (sin_2_t > 1.0) {
			return false;
		}

		if (into == 0) normal = normal * -1.0;

		double sin_t = sqrt(sin_2_t);
		double cos_t = sqrt(1 - sin_2_t);

		Vector3 tmp = (in_dir + normal * ddn);
		tmp.MakeUnitVector();
		Vector3 refr_direct = normal * (-1.0 * cos_t) + tmp * sin_t;
		refr_direct.MakeUnitVector();
		refraction = refr_direct;


        double F0 = (nc - nt) * (nc - nt) / ((nc + nt) * (nc + nt));

        double c = 1 - (into ? ddn : Dot(refraction, normal) * -1);

        Fe = F0 + (1 - F0) * c * c * c * c * c;
		
		return true;
	}

	rgb color;
	double nt;

};

