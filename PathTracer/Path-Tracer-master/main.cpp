#include "Ray.h"
#include "Image.h"
#include "Mesh.h"
#include "MeshTriangle.h"
#include "Shape.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Sample.h"
#include "Camera.h"
#include "MarbleTexture.h"
#include "SimpleTexture.h"
#include "DiffuseMaterial.h"
#include "NoiseTexture.h"
#include "DynSphere.h"
#include "ImageTexture.h"
#include "UVTriangle.h"
#include "PhoneMetalMaterial.h"
#include "DielectricMaterial.h"
#include "RefractMaterial.h"
#include "MixRefracflecMaterial.h"
#include "UVSphere.h"
#include "Instance.h"
#include <ctime>
#include "BVH.h"
#include "RNG.h"
#define OUTPUT_PATH "diff_spec.png"
#define CANEXCUTE false

const Vector3 cc(0.f, 0.f, -13.f);
const float cl = 10.f;

const float intensity = 30.0f;

const int num_samples = 32000;
const int image_size = 400;

RNG rng;



rgb radiance(const BVH *root, Ray ray, int depth = 0)
{
	depth++;
	HitRecord rec;
	bool hit = root->Hit(ray, 0.001f, 5000.f, 0.f, rec);
	if (!hit) { return rgb(); }
	rgb _emission = rec.material->EmissionResponse();
	rgb f = rec.material->AmbientResponse(rec.tex_pos, rec.uv);


	Vector2 seed;
	seed.Scramble();
	
	double p = fmax(fmax(f.r, f.g), f.b);

	if (depth > 5) {
		if (rng() < p) f = f * (1 / p);
		else return _emission;
	}

	if (depth > 100) {
		return _emission;
	}

	if (rec.material->IsDiffuse()) {
		rgb color;
		Vector3 d;
		rec.material->DiffuseDirection(rec.uvw, ray.d, rec.tex_pos, rec.uv, seed, color, d);
		Ray ray_diff;
		ray_diff.SetOrigin(rec.pos);
		ray_diff.SetDirection(d);
		return _emission + color*radiance(root, ray_diff, depth);
	}
	else if (rec.material->IsPhongMetal()) {
		rgb color;
		Vector3 d;
		if (rec.material->PhongMetalDirection(rec.uvw, ray.d, rec.tex_pos, rec.uv, seed, color, d)) {
			Ray ray_metal;
			ray_metal.SetOrigin(rec.pos);
			ray_metal.SetDirection(d);
			return _emission + color*radiance(root, ray_metal, depth);
		}
		else {
			return _emission;
		}
	}
	else if (rec.material->IsDielectric()) {
		rgb color;
		Vector3 d;
		rec.material->SpecularDirection(rec.uvw, ray.d, rec.tex_pos, rec.uv, seed, color, d);
		Ray ray_spec;
		ray_spec.SetOrigin(rec.pos);
		ray_spec.SetDirection(d);
		return _emission + radiance(root, ray_spec, depth);
	}
	else if (rec.material->IsRefract()) {
		rgb color;
		Vector3 d;
		if (rec.material->RefractDirection(rec.uvw, ray.d, rec.tex_pos, rec.uv, seed, color, d)) {
			Ray ray_refra;
			ray_refra.SetOrigin(rec.pos);
			ray_refra.SetDirection(d);
			return _emission + radiance(root, ray_refra, depth);
		} else {
			return _emission;
		}

	}
	else if (rec.material->IsMixRefractflec()) {
		rgb color;
		Vector3 refl, refr;
		float Fe;
		if (rec.material->RefractflecDirection(rec.uvw, ray.d, rec.tex_pos, rec.uv, seed, color, refr, refl, Fe)) {
			Ray ray_refl, ray_refr;
			ray_refl.SetOrigin(rec.pos); ray_refl.SetDirection(refl);
			ray_refr.SetOrigin(rec.pos); ray_refr.SetDirection(refr);

			if (depth > 2) {
				double P = 0.25 + 0.5 * Fe;
				if (rng() < P)
					return radiance(root, ray_refl, depth) * (Fe / P);
				else
					return radiance(root, ray_refr, depth) * ((1-Fe) / (1 - P));
			}
			else {
				return radiance(root, ray_refl, depth) * Fe + radiance(root, ray_refr, depth) * (1 - Fe);
			}
		}
		else { // don't have reflection
			Ray ray_refl;
			ray_refl.SetOrigin(rec.pos); ray_refl.SetDirection(refl);
			return _emission + f * radiance(root, ray_refl, depth);
		}
	}
	else {
		cerr << "not implemented" << endl;
	}
}





void render(const Camera &camera, int image_width, BVH *root, int nsamples, char *path)
{
	int width = camera.u1 - camera.u0;
	int height = camera.v1 - camera.v0;
	int image_height = image_width*height / width;
	double w = 1.0 / image_width;
	double h = 1.0 / image_height;
	double w100 = w * 100;
	double s = 1.0 / nsamples;
	Image image(image_width, image_height);

	int i, j;
	for (i = 0; i < image_width; i++)
	{
#pragma omp parallel for
		for (j = 0; j < image_height; j++)
		{
			Vector2 *pixel_samples = new Vector2[nsamples];
			Vector2 *lens_samples = new Vector2[nsamples];
			rgb color;
			MultiJitter(pixel_samples, nsamples);
			CubicSplineFilter(pixel_samples, nsamples);
			Jitter(lens_samples, nsamples);
			Shuffle(lens_samples, nsamples);
			HitRecord rec;
			Vector2 *ps = pixel_samples, *ls = lens_samples;
			for (; ps != pixel_samples + nsamples; ++ps, ++ls)
			{
				Ray ray = camera.GetRay((i + ps->x)*w, (j + ps->y)*h, ls->x, ls->y);
				color += radiance(root, ray);
			}
			image[i][j] = color*s;
                        //printf("value: %f\n", (image[i][j].r + image[i][j].g + image[i][j].b)/3.0);
			delete[] pixel_samples;
			delete[] lens_samples;
		}
		std::cout << "\b\b\b\b\b" << int(i*w100) << "%";
	}

	ofstream outfile(path, ios::binary);
	image.WritePNG(outfile);
	outfile.close();
}



int main()
{
	srand(time(nullptr));
	SimpleTexture white(rgb(1.f, 1.f, 1.f));
	SimpleTexture black((rgb()));
	SimpleTexture red(rgb(1.f, .0f, .0f));
	SimpleTexture green(rgb(.0f, 1.f, .0f));
	SimpleTexture blue(rgb(.0f, .0f, 1.f));
	SimpleTexture gray(rgb(.75f));
	SimpleTexture lightred(rgb(.75f, .25f, .25f));
	SimpleTexture lightblue(rgb(.25f, .25f, .75f));
	SimpleTexture refl(rgb(0.999f));
	SimpleTexture phong(rgb(16.f));
	ImageTexture image("checkerboard.ppm");

	DiffuseMaterial rm(&red);
	DiffuseMaterial gm(&green);
	DiffuseMaterial bm(&blue);
	DiffuseMaterial lrm(&lightred);
	DiffuseMaterial lbm(&lightblue);
	DiffuseMaterial graym(&gray);

	DiffuseMaterial blackm(&black);
	blackm.setEmission(intensity);

	DiffuseMaterial imm(&image);
	PhongMetalMaterial pmm(&refl, &phong);
	DielectricMaterial dm;
	RefractMaterial rmm(1.5);
	MixRefractflecMaterial mmm(1.5);

	Mesh mesh;
	mesh.material = &graym;
	mesh.vertUVs = new VertexUV[8]{
		{ Vector3(cc.x + cl, cc.y + cl, cc.z - cl), Vector2() },
		{ Vector3(cc.x - cl, cc.y + cl, cc.z - cl), Vector2() },
		{ Vector3(cc.x - cl, cc.y + cl, cc.z + 2 * cl), Vector2() },
		{ Vector3(cc.x + cl, cc.y + cl, cc.z + 2 * cl), Vector2() },

		{ Vector3(cc.x + cl, cc.y - cl, cc.z - cl), Vector2() },
		{ Vector3(cc.x - cl, cc.y - cl, cc.z - cl), Vector2() },
		{ Vector3(cc.x - cl, cc.y - cl, cc.z + 2 * cl), Vector2() },
		{ Vector3(cc.x + cl, cc.y - cl, cc.z + 2 * cl), Vector2() }
	};

	Mesh mesh2;
	mesh2.material = &lrm;
	mesh2.vertUVs = new VertexUV[8]{
		{ Vector3(cc.x + cl, cc.y + cl, cc.z - cl), Vector2() },
		{ Vector3(cc.x - cl, cc.y + cl, cc.z - cl), Vector2() },
		{ Vector3(cc.x - cl, cc.y + cl, cc.z + 2 * cl), Vector2() },
		{ Vector3(cc.x + cl, cc.y + cl, cc.z + 2 * cl), Vector2() },

		{ Vector3(cc.x + cl, cc.y - cl, cc.z - cl), Vector2() },
		{ Vector3(cc.x - cl, cc.y - cl, cc.z - cl), Vector2() },
		{ Vector3(cc.x - cl, cc.y - cl, cc.z + 2 * cl), Vector2() },
		{ Vector3(cc.x + cl, cc.y - cl, cc.z + 2 * cl), Vector2() }
	};

	Mesh mesh3;
	mesh3.material = &lbm;
	mesh3.vertUVs = new VertexUV[8]{
		{ Vector3(cc.x + cl, cc.y + cl, cc.z - cl), Vector2() },
		{ Vector3(cc.x - cl, cc.y + cl, cc.z - cl), Vector2() },
		{ Vector3(cc.x - cl, cc.y + cl, cc.z + 2 * cl), Vector2() },
		{ Vector3(cc.x + cl, cc.y + cl, cc.z + 2 * cl), Vector2() },

		{ Vector3(cc.x + cl, cc.y - cl, cc.z - cl), Vector2() },
		{ Vector3(cc.x - cl, cc.y - cl, cc.z - cl), Vector2() },
		{ Vector3(cc.x - cl, cc.y - cl, cc.z + 2 * cl), Vector2() },
		{ Vector3(cc.x + cl, cc.y - cl, cc.z + 2 * cl), Vector2() }
	};
	Shape *shapes[] = {
		
		new Sphere(Vector3(0.f, 17.875f, -5.f), 8.125f, &blackm),
		new UVSphere(Vector3(-3.f, -5.f, -7.f), 3.f, &dm),
		// new Sphere(Vector3(3.f, -5.f, -6.f), 3.f, &dm),
		
		new Sphere(Vector3(3.f, -5.f, -6.f), 2.f, &mmm),

		new MeshTriangleUV(&mesh, 1, 0, 3, 0),
		new MeshTriangleUV(&mesh, 2, 1, 3, 0),
		new MeshTriangleUV(&mesh2, 5, 1, 2, 0),
		new MeshTriangleUV(&mesh2, 6, 5, 2, 0),
		new MeshTriangleUV(&mesh, 4, 0, 1, 0),
		new MeshTriangleUV(&mesh, 5, 4, 1, 0),
		new MeshTriangleUV(&mesh3, 3, 0, 4, 0),
		new MeshTriangleUV(&mesh3, 7, 3, 4, 0),
		new MeshTriangleUV(&mesh, 7, 4, 5, 0),
		new MeshTriangleUV(&mesh, 6, 7, 5, 0)

	};

	BVH root(shapes, sizeof(shapes) / sizeof(shapes[0]));

	Camera camera(Vector3(0.f, 0.f, 7.f), Vector3(0.f, 0.f, -1.f), Vector3(0.f, 1.f, 0.f), .1f, -7.f, 7.f, -7.f, 7.f, 7.f);

	render(camera, image_size, &root, num_samples, OUTPUT_PATH);

	return 0;
}
