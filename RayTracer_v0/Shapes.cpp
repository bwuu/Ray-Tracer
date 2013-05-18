#include "stdafx.h"
#include "Shapes.h"
#include "RayTraceStructs.h"


Vec3 Shape::get_amb_mat() 
{ return shape_material.mat_ambient; }

Vec3 Shape::get_diff_mat() 
{ return shape_material.mat_diffuse; }

Vec3 Shape::get_spec_mat() 
{ return shape_material.mat_specular; }

double Shape::get_reflection()
{ return shape_material.refl; }

double Shape::get_transparency()
{ return shape_material.transparency; }

double Shape::get_refraction()
{ return shape_material.refraction; }

//  SPHERE
Sphere::Sphere (Vec3 icentre, double iradius, Vec3 ambient, Vec3 diffuse, Vec3 specular, double refl, double transp, double refr)
{
	centre = icentre;
	radius = iradius;
	shape_material.mat_ambient = ambient;
	shape_material.mat_diffuse = diffuse;
	shape_material.mat_specular = specular;
	shape_material.refl = refl;
	shape_material.transparency = transp;
	shape_material.refraction = refr;
}

bool Sphere::check_intersect (ShadowRay inc)
{
	Vec3 local_orig = inc.orig - centre;
	const double A = 1.0000;
	const double B = 2 * dot(inc.dir, local_orig);
	const double C = local_orig.sqr_mag() - radius * radius;

	const double det = B*B - 4*A*C;
	if(det < 0.0) return false;

	const double q = (B < 0.0 ? -B + sqrt(det) : -B - sqrt(det));

	const double t1 = q / (2 * A);
	const double t2 = (2 * C) / q;
	
	if (inc.infinite) {
		if (t1 < 0.0 && t2 < 0.0) return false;
		else return true;
	}
	// else
	if (t1 < 0.0)
	{
		if (t2 < 0.0) return false;
		if (t2*t2 * A > inc.sqr_dist) return false;
	}
	if (t1 >= 0.0)
	{
		if (t2 >= 0.0 && pow(std::min(t1, t2), 2) * A > inc.sqr_dist) return false;
		if (t1*t1 * A > inc.sqr_dist) return false;
	}
	return true;
}


bool Sphere::get_intersect(Ray inc, Intersection& res)
{
	Vec3 local_orig = inc.orig - centre;
	const double A = 1.0000;
	const double B = 2 * dot(inc.dir, local_orig);
	const double C = local_orig.sqr_mag() - radius * radius;

	const double det = B * B - 4 * A * C;
	if(det < 0.0) return false;

	const double q = (B < 0.0 ? -B + sqrt(det) : -B - sqrt(det));

	const double t1 = q / (2 * A);
	const double t2 = 2 * C / q; 
	double t;

	if (t1 >= 0.0)
	{
		if (t2 >= 0.0) t = std::min(t1, t2);
		else t = t1;
	}
	else if (t2 >= 0.0) t = t2;
	else return false;

	Vec3 hitpoint = inc.orig + inc.dir * t;

	res.pos = hitpoint;
	res.normal = (hitpoint - centre).unit_vec();
	res.shape_ref = this;
	res.dir = inc.dir;
	res.dist = t * inc.dir.mag(); // can opt
	return true;
}


//  TRIANGLE
Triangle::Triangle (Vec3 v1, Vec3 v2, Vec3 v3, Vec3 amb, Vec3 diff, Vec3 spec, double refl, double transp, double refr)
{
	vertices[0] = v1;
	vertices[1] = v2;
	vertices[2] = v3;
	plane_normal = cross(v2 - v1, v3 - v1).unit_vec();
	shape_material.mat_ambient = amb;
	shape_material.mat_diffuse = diff;
	shape_material.mat_specular = spec;
	shape_material.refl = refl;
	shape_material.transparency = transp;
	shape_material.refraction = refr;
}


bool Triangle::check_intersect(ShadowRay inc)
{
	double param = dot(vertices[0] - inc.orig, plane_normal) / dot(inc.dir, plane_normal);
	if (param < 0.0) return false;
	if (!inc.infinite && inc.sqr_dist < (inc.dir * param).sqr_mag()) return false;

	Vec3 point (inc.orig + inc.dir * param);

    Vec3 v0 = vertices[1] - vertices[0];
	Vec3 v1 = vertices[2] - vertices[0];
	Vec3 v2 = point - vertices[0];

    double d00 = dot(v0, v0);
    double d01 = dot(v0, v1);
    double d11 = dot(v1, v1);
    double d20 = dot(v2, v0);
    double d21 = dot(v2, v1);
    double denom = d00 * d11 - d01 * d01;

    double u = (d11 * d20 - d01 * d21) / denom;
    double v = (d00 * d21 - d01 * d20) / denom;
    double w = 1.0 - u - v;

	if (u < 0.0 || v < 0.0 || w < 0.0) return false;
	return true;
}

bool Triangle::get_intersect(Ray inc, Intersection& res)
{
	double param = dot(vertices[0] - inc.orig, plane_normal) / dot(inc.dir, plane_normal);
	if (param < 0.0) return false;
	Vec3 point (inc.orig + inc.dir * param);

    Vec3 v0 = vertices[1] - vertices[0];
	Vec3 v1 = vertices[2] - vertices[0];
	Vec3 v2 = point - vertices[0];

    double d00 = dot(v0, v0);
    double d01 = dot(v0, v1);
    double d11 = dot(v1, v1);
    double d20 = dot(v2, v0);
    double d21 = dot(v2, v1);
    double denom = d00 * d11 - d01 * d01;

    double u = (d11 * d20 - d01 * d21) / denom;
    double v = (d00 * d21 - d01 * d20) / denom;
    double w = 1.0 - u - v;

	if (u < 0.0 || v < 0.0 || w < 0.0) return false;

	res.pos = point;
	res.normal = plane_normal;
	res.dir = inc.dir;
	res.shape_ref = this;
	res.dist = (inc.dir * param).mag(); // can opt
	return true;
}

sphTriangle::sphTriangle (Vec3 verts[], Vec3 norms[])
{
	for (int i = 0; i < 3; i++)
	{
		vertices[i] = verts[i];
		normals[i] = norms[i];
	}
	plane_normal = cross(vertices[1] - vertices[0], vertices[2] - vertices[0]).unit_vec();
}

bool sphTriangle::check_intersect(ShadowRay inc)
{
	double param = dot(vertices[0] - inc.orig, plane_normal) / dot(inc.dir, plane_normal);
	if (param < 0.0) return false;
	if (!inc.infinite && inc.sqr_dist < (inc.dir * param).sqr_mag()) return false;

	Vec3 point (inc.orig + inc.dir * param);

    Vec3 v0 = vertices[1] - vertices[0];
	Vec3 v1 = vertices[2] - vertices[0];
	Vec3 v2 = point - vertices[0];

    double d00 = dot(v0, v0);
    double d01 = dot(v0, v1);
    double d11 = dot(v1, v1);
    double d20 = dot(v2, v0);
    double d21 = dot(v2, v1);
    double denom = d00 * d11 - d01 * d01;

    double u = (d11 * d20 - d01 * d21) / denom;
    double v = (d00 * d21 - d01 * d20) / denom;
    double w = 1.0 - u - v;

	if (u < 0.0 || v < 0.0 || w < 0.0) return false;
	return true;
}

bool sphTriangle::get_intersect(Ray inc, Intersection& res)
{
	double param = dot(vertices[0] - inc.orig, plane_normal) / dot(inc.dir, plane_normal);
	if (param < 0.0) return false;
	Vec3 point (inc.orig + inc.dir * param);

    Vec3 v0 = vertices[1] - vertices[0];
	Vec3 v1 = vertices[2] - vertices[0];
	Vec3 v2 = point - vertices[0];

    double d00 = dot(v0, v0);
    double d01 = dot(v0, v1);
    double d11 = dot(v1, v1);
    double d20 = dot(v2, v0);
    double d21 = dot(v2, v1);
    double denom = d00 * d11 - d01 * d01;

    double u = (d11 * d20 - d01 * d21) / denom;
    double v = (d00 * d21 - d01 * d20) / denom;
    double w = 1.0 - u - v;

	if (u < 0.0 || v < 0.0 || w < 0.0) return false;

	res.pos = point;
	res.normal = (normals[0] * w + normals[1] * u + normals[2] * v).unit_vec();
	res.dir = inc.dir;
	res.shape_ref = this;
	// needs dist
	return true;
}

/*
Vec3 vorig1 (0.0, 0.0, 0.0);
Sphere s1(0.0, 0.0, -2.0, 1.8);
Sphere s2(0.0, 0.0, -5.0, 1.0);

Vec3 vdir1 (1.0, 0.0, -2.0);
Vec3 vdir2 (1.0, 1.0, -2.0);
Ray r1(vorig1, vdir1);
ShadowRay sr1(vorig1, vdir1);
ShadowRay sr2(vorig1, -vdir1);
ShadowRay sr3(vorig1, vdir2);
ShadowRay sr4(vorig1, -vdir2);
ShadowRay fsr1(vorig1, vdir1, 0.1);
ShadowRay fsr2(vorig1, vdir2, 100);
ShadowRay fsr3(vorig1, -vdir2, 100);

Vec3 vert1(0.0, 0.0, -1.0);
Vec3 vert2(1.0, 1.0, -1.0);
Vec3 vert3(0.0, 1.0, -1.0);
Triangle t1 (vert1, vert2, vert3);

TEST (I_SECT_SPH) {
	CHECK(s1.check_intersect(sr1));
	CHECK(!s1.check_intersect(sr2));
	CHECK(!s2.check_intersect(sr1));
}

TEST (I_SECT_TRI) {
	CHECK(!t1.check_intersect(sr1));
	CHECK(t1.check_intersect(sr3));
	CHECK(!t1.check_intersect(sr4));
}

TEST (F_SECT_SPH) {
	CHECK(!s1.check_intersect(fsr1));
	CHECK(s1.check_intersect(fsr2));
	CHECK(!s1.check_intersect(fsr3));
}

TEST (F_SECT_TRI) {
	CHECK(!t1.check_intersect(fsr1));
	CHECK(t1.check_intersect(fsr2));
	CHECK(!t1.check_intersect(fsr3));
}
*/