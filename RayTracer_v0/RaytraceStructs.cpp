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

RegCast::RegCast(double il, double ir, double iu, double id, double idist, int ixres)
{
	l = il;
	r = ir;
	u = iu;
	d = id;
	plane_dist = idist;

	Vec3 origin(0.0, 0.0, 0.0);
	pos = origin;
	w = ixres;
	pixel_size = (r - l) / w;

	assert(pixel_size > 0.0);

	h = static_cast<int>((u - d) / pixel_size);

	Vec3 iLL (l + pixel_size * 0.5, d + pixel_size * 0.5, -plane_dist);
	LL = iLL;
}


bool RegCast::raytrace_scene(Scene& scene)
{
	FILE * f;
	unsigned char *img = NULL;
	assert(w > 0 && h > 0);
	int filesize = 54 + 3 * w * h;
	if (img) free( img );
	img = (unsigned char *)malloc(3*w*h);
	memset(img,0,sizeof(img));

	int ii = 0;

	for (int i = 0; i < h; i++) 
	{
		for (int j = 0; j < w; j++)
		{
			Vec3 rayvec = LL;
			// lol so bad
			rayvec.x[0] += j * pixel_size;
			rayvec.x[1] += i * pixel_size;
			Ray lightray (pos + rayvec, rayvec);
			RGB pxcol = scene.trace(lightray, 1.0, 0);

			assert(pxcol.x[0] <= 1.0 && pxcol.x[1] <= 1.0 && pxcol.x[2] <= 1.0);

			img[ii + 2] = static_cast<unsigned int>(pxcol.x[0] * 255);
			img[ii + 1] = static_cast<unsigned int>(pxcol.x[1] * 255);
			img[ii    ] = static_cast<unsigned int>(pxcol.x[2] * 255);
			ii += 3;
		}
	}

	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
	unsigned char bmppad[3] = {0,0,0};

	bmpfileheader[ 2] = (unsigned char)(filesize    );
	bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
	bmpfileheader[ 4] = (unsigned char)(filesize>>16);
	bmpfileheader[ 5] = (unsigned char)(filesize>>24);

	bmpinfoheader[ 4] = (unsigned char)(       w    );
	bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
	bmpinfoheader[ 6] = (unsigned char)(       w>>16);
	bmpinfoheader[ 7] = (unsigned char)(       w>>24);
	bmpinfoheader[ 8] = (unsigned char)(       h    );
	bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
	bmpinfoheader[10] = (unsigned char)(       h>>16);
	bmpinfoheader[11] = (unsigned char)(       h>>24);

	f = fopen("img.bmp","wb");
	fwrite(bmpfileheader,1,14,f);
	fwrite(bmpinfoheader,1,40,f);
	for(int i=0; i<h; i++)
	{
		fwrite(img+(w*(i)*3),3,w,f);
		fwrite(bmppad,1,(4-(w*3)%4)%4,f);
	}
	fclose(f);
	free(img);

	return true;
}