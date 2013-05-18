
#include "stdafx.h"
#include "Scene.h"

const int MAX_DEPTH = 5;
const double MIN_INTENSITY = 0.05;
const double MAX_DIST = 10000;

Scene::Scene ()
{
	active_prims = 0;
	active_lights = 0;
}

void Scene::add_primitive(Shape* sp) 
{
	//  add error catching for exceeding max
	prim_refs[active_prims] = sp;
	active_prims++;
}

void Scene::add_lightsource(LightSource* lp) 
{
	//  add error catching for exceeding max
	light_refs[active_lights] = lp;
	active_lights++;
}

bool Scene::check_shadowing(LightSource* light_ref, Vec3 point, Vec3& dir)
{
	ShadowRay darkray = light_ref->get_shadowray(point);
	//std::cout << "shadowray orig:" << darkray.orig.x[0] << " " << darkray.orig.x[1] << " " << darkray.orig.x[2] << "\n";
	//std::cout << "shadowray dir:" << darkray.dir.x[0] << " " << darkray.dir.x[1] << " " << darkray.dir.x[2] << "\n";
	dir = darkray.dir;
	for(int i = 0; i < active_prims; i++)
	{
		if(prim_refs[i]->check_intersect(darkray)) return true;
	}
	return false;
}

RGB Scene::trace(Ray ray, double ray_intensity, int trace_depth)
{
	RGB result(0.0, 0.0, 0.0);
	if (trace_depth > MAX_DEPTH) return (result);
	if (ray_intensity < MIN_INTENSITY) return (result);

	Intersection min_hit;
	min_hit.dist = MAX_DIST;
	bool intersected = false;

	for(int i = 0; i < active_prims; i++)
	{
		Intersection current_hit;
		if(prim_refs[i]->get_intersect(ray, current_hit)
			&& current_hit.dist < min_hit.dist) 
		{
			min_hit = current_hit;
			intersected = true;
		}
	}
	//std::cout << "intersection pos:" << intersect.pos.x[0] << " " << intersect.pos.x[1] << " " << intersect.pos.x[2] << "\n";

	//if(!intersected) std::cout << "wtf negz no hit\n";
	//  returns pure black
	if (!intersected) return (result);

	//  slight adjustment to prevent floating-point inaccuracy from 
	//  causing unwanted collisions
	min_hit.pos += min_hit.normal * 0.0001;

	//if (intersected) std::cout << "adjusted min_hition pos:" << min_hit.pos.x[0] << " " << min_hit.pos.x[1] << " " << min_hit.pos.x[2] << "\n";
	for (int i = 0; i < active_lights; i++)
	{
		Vec3 lightray;
		//  deposits lightray in lightray variable
		bool shadowed = check_shadowing(light_refs[i], min_hit.pos, lightray);
		if (!shadowed)
		{
			// currently ignores color of light
			result += min_hit.shape_ref->get_diff_mat() 
				* std::max( dot(min_hit.normal, lightray), 0.0 );

			//  reflection
			Vec3 R = lightray - min_hit.normal * 2 * dot( lightray, min_hit.normal );
			double specdot = dot( R, min_hit.dir );
			if (specdot > 0)
				result += min_hit.shape_ref->get_spec_mat() * pow( specdot, 20 );
		}
	//if(shadowed) std::cout <<" wtf negz shadowed at:" << min_hit.pos.x[0] << " " << min_hit.pos.x[1] << " " << min_hit.pos.x[2] << "\n";
	//else std::cout << "plane normal: " << min_hit.dir.x[0] << " " << min_hit.dir.x[1] << " " << min_hit.dir.x[2] << "\n";
	//else std::cout << "dot result: " << dot(min_hit.dir, lposdir) << "\n";
	}
	
	//  reflection
	double obj_refl = min_hit.shape_ref->get_reflection();
	if (obj_refl > 0.0) {
		Vec3 refl_dir = ray.dir - min_hit.normal * 2 * dot( ray.dir, min_hit.normal );
		Ray refl_ray ( min_hit.pos, refl_dir );
		result += trace( refl_ray, obj_refl * ray_intensity, trace_depth + 1 );
	}

	//  refraction
	double obj_transp = min_hit.shape_ref->get_transparency();
	if (obj_transp > 0.0) {
		//  check that when transp not 0, refr not 0.
		double obj_refr = min_hit.shape_ref->get_refraction();
		//  check htis
		double cos_term = dot( ray.dir, min_hit.normal ) ;
		double nr = (cos_term < 0.0) ? (1 / obj_refr) : (obj_refr);
		double sin0t2 = nr*nr * (1 - pow(cos_term, 2));
		if (sin0t2 < 1.0)
		{
			Vec3 refracted_dir = ray.dir * nr
				- min_hit.normal * (nr * abs(cos_term) - sqrt(1 - sin0t2));
			assert(refracted_dir.mag() > 0.0);
			Vec3 irpos = (cos_term < 0.0) ? 
				(min_hit.pos - min_hit.normal * 0.0002) : (min_hit.pos);
			Ray refr_ray ( irpos, refracted_dir);
			result += trace( refr_ray, obj_transp * ray_intensity, trace_depth + 1 );
		}
	}

	for (int i = 0; i < 3; i++) result.x[i] = std::min(result.x[i], 1.0);

	assert(ray_intensity <= 1.0);

	return (result * ray_intensity);
}
