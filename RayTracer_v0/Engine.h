#ifndef RT_ENGINE
#define RT_ENGINE

class Scene;

#include "Vec3.h"

// Ray casting engine.
class RayCastEngine
{
protected:
	//  image resolution
	int w, h;
};

//  standard casting mode
class RegCast: public RayCastEngine
{
private:
	//  camera position
	Vec3 pos;
	//  orthogonal distance between viewing plane and camera
	double plane_dist;
	//  boundaries of viewing plane
	double l, r, u, d;
	//  width/height of pixel; calculated internally
	double pixel_size;
	//  centre of lower left pixel; reduces calculation for ray casts
	Vec3 LL;
public:
	RegCast (double l, double r, double u, double d, double plane_dist, int x_resolution);

	//  raytraces the scene and puts result in "img.bmp"
	bool raytrace_scene(Scene& theScene);
};


#endif