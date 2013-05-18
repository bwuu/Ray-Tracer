#ifndef RT_STRUCTS
#define RT_STRUCTS

#include "Vec3.h"
class Scene;
class Shape;

//  Ray Structs
struct Ray {
	Vec3 orig;
	Vec3 dir;
	Ray ();
	Ray (Vec3 init_orig, Vec3 init_dir);
};

//  Rays for shadow checking
struct ShadowRay : public Ray {
	//  determines if ray is segment/line, corresponding to
	//  (local)/(at infinity) light source.
	bool infinite;
	double sqr_dist;
	ShadowRay(Vec3 orig, Vec3 dir);
	ShadowRay(Vec3 orig, Vec3 dir, double raylen);
};

struct Intersection
{
	Vec3 pos;
	Vec3 normal;
	Vec3 dir;
	Shape* shape_ref;
	double dist;
};

// LIGHTING STRUCTS //

class LightSource
{
public:
	virtual ShadowRay get_shadowray(Vec3)=0;
};

class PointLight : public LightSource
{
private:
	Vec3 pos;
	RGB color;
public:
	PointLight ();
	// intensity forced within range 0-1
	PointLight (Vec3 pos); //  <- color set to white
	PointLight (Vec3 pos, RGB color);
	RGB get_color();
	ShadowRay get_shadowray(Vec3 point);
};

// viewpoint structs
class RayCastEngine
{
public:
	//  image resolution
	int w, h;
};

class RegCast: public RayCastEngine
{
	//  camera position
	Vec3 pos;
	//  distance of viewing plane
	double plane_dist;
	//  boundaries of viewing plane
	double l, r, u, d;
	//  width/height of pixel; calculated internally
	double pixel_size;
	//  centre of lower left pixel
	Vec3 LL;
public:
	RegCast (double l, double r, double u, double d, double plane_dist, int x_resolution);
	bool raytrace_scene(Scene& theScene);
};


#endif