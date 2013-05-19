#ifndef RT_STRUCTS
#define RT_STRUCTS

//  CONTAINS STRUCTURES AND CLASSES FOR RAY CASTING, INTERSECTION
//  CALCULATION AND LIGHTING CALCUATIONS

#include "Vec3.h"
class Scene;
class Shape;

//  Rays for ray casting
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

//  holds data describing a ray-object intersection event
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
	//  returns shadowray to use for shadow checks
	virtual ShadowRay get_shadowray(Vec3 point)=0;
};

class PointLight : public LightSource
{
private:
	Vec3 pos;
	RGB color;
public:
	PointLight ();
	// intensity forced within range 0-1
	//  sets color to white by default
	PointLight (Vec3 pos);
	PointLight (Vec3 pos, RGB color);
	RGB get_color();
	//  returns shadowray to use for shadow checks
	ShadowRay get_shadowray(Vec3 point);
};



#endif