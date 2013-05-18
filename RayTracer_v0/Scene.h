#ifndef SCENE
#define SCENE

#include "Vec3.h"
#include "RayTraceStructs.h"
#include "Shapes.h"

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
	void add_primitive(Shape* shape_p);
	void add_lightsource(LightSource* lightsource_p);

	RGB trace(Ray ray, double ray_intensity, int trace_depth);

	bool check_shadowing(LightSource* lightsource_p, Vec3 point, Vec3& point_to_light_dir);
};



#endif