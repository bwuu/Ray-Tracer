#include "stdafx.h"
#include "RayTraceStructs.h"
#include "Scene.h"

//  RAYS
Ray::Ray() {}

Ray::Ray (Vec3 iorig, Vec3 idir) {
	orig = iorig;
	dir = idir.unit_vec();
}

ShadowRay::ShadowRay(Vec3 iorig, Vec3 idir)
{
	orig = iorig;
	dir = idir.unit_vec();
	infinite = true;
}

ShadowRay::ShadowRay(Vec3 iorig, Vec3 idir, double isqr_dist)
{
	orig = iorig;
	// remv unit vec
	dir = idir.unit_vec();
	sqr_dist = isqr_dist;
	infinite = false;
}

//  LIGHTS
PointLight::PointLight () {}

RGB PointLight::get_color() { return color; }

PointLight::PointLight (Vec3 ipos)
{
	pos = ipos;
	Vec3 i (1.0, 1.0, 1.0);
	color = i;
}

PointLight::PointLight (Vec3 ipos, RGB icolor)
{
	pos = ipos;
	color = icolor;
}

ShadowRay PointLight::get_shadowray(Vec3 point)
{
	ShadowRay darkray(point, (pos - point), (pos-point).sqr_mag() );
	return (darkray);
}

