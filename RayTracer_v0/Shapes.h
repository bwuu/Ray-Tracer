#ifndef SHAPES
#define SHAPES

struct Ray;
struct ShadowRay;
struct Intersection;

#include "Vec3.h"

struct Material
{
	Vec3 mat_ambient;
	Vec3 mat_diffuse;
	Vec3 mat_specular;
	double refl;
	double transparency;
	double refraction;
};

class Shape {
protected:
	Material shape_material;
public:
	virtual bool get_intersect(Ray ray, Intersection& result)=0;
	virtual bool check_intersect(ShadowRay shadowray)=0;
	Vec3 get_amb_mat();
	Vec3 get_diff_mat();
	Vec3 get_spec_mat();
	double get_reflection();
	double get_transparency();
	double get_refraction();
};

class Sphere: public Shape 
{
private:
	Vec3 centre;
	double radius;
public:
	Sphere (Vec3 centre, double radius, Vec3 amb_mat, Vec3 diff_mat, Vec3 spec_mat, double refl, double transp, double refraction);

	bool check_intersect(ShadowRay shadowray);
	bool get_intersect(Ray ray, Intersection& result);
};

class Triangle: public Shape
{
private:
	Vec3 vertices[3];
	Vec3 plane_normal;
public:
	Triangle (Vec3 v1, Vec3 v2, Vec3 v3, Vec3 amb, Vec3 diff, Vec3 spec, double refl, double transparency, double refraction);

	bool check_intersect(ShadowRay);
	bool get_intersect(Ray, Intersection&);
};

//  triangle with specified normals for each vertex.
class sphTriangle: public Shape
{
	Vec3 vertices[3];
	Vec3 plane_normal;
	Vec3 normals[3];
public:
	//  define these
	//  figure out how to deal with defining normals AND all color matrices
	sphTriangle (Vec3, Vec3, Vec3, Vec3, Vec3, Vec3);
	sphTriangle (Vec3[], Vec3[]);
	bool check_intersect(ShadowRay);
	bool get_intersect(Ray, Intersection&);
};

#endif