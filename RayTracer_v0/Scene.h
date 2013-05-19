#ifndef SCENE
#define SCENE

class Shape;
class LightSource;
class Ray;

#include "Vec3.h"

class Scene
{
private:
	//  add primitive limits, same for lights
	//  primitive storage
	int active_prims;
	Shape* prim_refs[10];

	//  lightsources storage
	int active_lights;
	LightSource* light_refs[10];
public:
	Scene ();
	void add_primitive(Shape& shape);
	void add_lightsource(LightSource& lightsource);

	RGB trace(Ray ray, double ray_intensity, int trace_depth);

	bool check_shadowing(LightSource* lightsource_p, Vec3 point, Vec3& point_to_light_dir);
};



#endif