#include "stdafx.h"
#include "Engine.h"
#include "Scene.h"
#include "RayTraceStructs.h"

RegCast::RegCast(double il, double ir, double iu, double id, double idist, int x_resolution)
{
	//  init viewing plane properties
	l = il;
	r = ir;
	u = iu;
	d = id;
	plane_dist = idist;

	//  camera set at origin
	Vec3 origin(0.0, 0.0, 0.0);
	pos = origin;

	//  init resolution, calculate pixel size
	w = x_resolution;
	pixel_size = (r - l) / w;
	assert(pixel_size > 0.0);
	h = static_cast<int>((u - d) / pixel_size);

	//  calculate and store centre of lower left pixel
	Vec3 iLL (l + pixel_size * 0.5, d + pixel_size * 0.5, -plane_dist);
	LL = iLL;
}

//  trace scene and store resulting image as "img.bmp"
bool RegCast::raytrace_scene(Scene& scene)
{
	FILE * f;
	int filesize = 54 + 3 * w * h;

	//  initialize a memory block to hold bmp-format data
	unsigned char *img = NULL;
	if (img) free( img );
	img = (unsigned char *)malloc(3*w*h);
	memset(img,0,sizeof(img));

	//  trace scene and store image data in the prepared array
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

	//  initialize a few arrays to simplify bmp writing
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

	//  open "img.bmp" and write in image data
	f = fopen("img.bmp","wb");
	fwrite(bmpfileheader,1,14,f);
	fwrite(bmpinfoheader,1,40,f);
	for(int i=0; i<h; i++)
	{
		fwrite(img+(w*(i)*3),3,w,f);
		fwrite(bmppad,1,(4-(w*3)%4)%4,f);
	}

	//  close file, free memory block and return success!
	fclose(f);
	free(img);
	return true;
}