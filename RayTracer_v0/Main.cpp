
#include "stdafx.h"

#include "RayTraceStructs.h"
#include "Scene.h"
#include <conio.h>

//  Possible problem causes:
//  objects not showing - did you consider the maximum render distance?

//  TODO LIST //
//  restrict colors to lie in range 0-1
//  change asserts to error trhows
//  remove testing code (it is currently commented out)
//  check all constructors and normals dirs and normals
//  make everything (ie member access) more readable
//  -change private to protected in appropriate places
//  make structs more secure -ie private - MAKE RAYS PRIVATE
//  make all shapes include colors in constructors
//  optimize all normalizations, ie in the itnersection creation code inside shapes get_intersect

int main()
{
	Scene theScene;

	Vec3 zeros (0.0, 0.0, 0.0);
	Vec3 spos1 (0.4, 0.2, -1.7);
	Vec3 spos2 (-1.0, -.2, -2.5);
	Vec3 white (1.0, 1.0, 1.0);
	Vec3 purple (0.6, 0.2, 0.6);
	Vec3 grey (0.3, 0.3, 0.3);
	Sphere S1(spos1, 0.4, zeros, zeros, white, 0.0, 1.0, 1.01);
	theScene.add_primitive(&S1);
	Sphere S2(spos2, 0.4, zeros, zeros, white, 0.0, 1.0, 1.01);
	theScene.add_primitive(&S2);

	Vec3 v1 (1000.0, -2.0, -1000.0);
	Vec3 v2 (-1000.0, -2.0, -1000.0);
	Vec3 v3 (-1000.0, -2.0, 1000.0);
	Vec3 v4 (1000.0, -2.0, 1000.0);
	Triangle T1 (v1, v2, v3, zeros, white, zeros, 0.0, 0.0, 2.0);
	theScene.add_primitive(&T1);
	Triangle T2 (v1, v3, v4, zeros, white, zeros, 0.0, 0.0, 2.0);
	theScene.add_primitive(&T2);

	Vec3 lpos1(0.0, 500.0, -2.0);
	Vec3 lpos2(-4.0, 0.0, -2.0);
	PointLight L1(lpos1);
	PointLight L2(lpos2);
	theScene.add_lightsource(&L1);
	//theScene.add_lightsource(&L2);

	/*
	Vec3 origin (0.0, 0.0, 0.0);
	Vec3 negz (-1.0, -0.8, -1.0);
	Ray testray(origin, negz);
	Vec3 testres = theScene.trace(testray, 0);
	std::cout<< "coloroftestray:" << testres.x[0] << " " << testres.x[1] << " " << testres.x[2] << "\n";
	*/
	
	RegCast engine(-1, 1, 1, -1, 1, 1200);
	engine.raytrace_scene(theScene);

	raven::set::UnitTest::RunAllTests();
	_getch();
}