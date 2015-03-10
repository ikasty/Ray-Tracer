#include <stdio.h>
#include <string.h>
#include "type.h"


msl_ray gen_ray(camera msl_cam,int current_x,int current_y)
{
	msl_ray make_ray;
	int get_dir;
	float index[3];
	
	index[0]=current_x;
	index[1]=current_y;
	index[2]=msl_cam.distance;
		
	for(get_dir=0;get_dir<3;get_dir++)
	{
		make_ray.orig[get_dir] = msl_cam.orig[get_dir];
		make_ray.dir[get_dir] =index[get_dir] - make_ray.orig[get_dir];
	}

	return make_ray;
}
