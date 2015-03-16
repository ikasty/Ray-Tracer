#include <stdio.h>
#include <string.h>
#include "type.h"
#include <math.h>
hit intersect_triangle(msl_ray ray, triangle triangle);

int shadow_test(msl_ray ray, float l_source[3],triangle triangle)
{
	msl_ray shadow_ray;
	hit hit_result;
	
//	int exist_shadow;
	float shadow_XYZ[3];
	float shadow_t;
//	float intersect_t;
	int repetition;

	
	shadow_t=-(ray.orig[1]/ray.dir[1]);
	

	//computing shadow's x,y,z coordinate
	shadow_XYZ[0]=ray.orig[0]+(shadow_t * ray.dir[0]);
	shadow_XYZ[1]=0;
	shadow_XYZ[2]=ray.orig[2]+(shadow_t * ray.dir[2]);

	for(repetition=0;repetition<3;repetition++)
	{
		shadow_ray.orig[repetition]=shadow_XYZ[repetition];
		shadow_ray.dir[repetition]=l_source[repetition]-shadow_XYZ[repetition];
	}
	hit_result.t=intersect_triangle(shadow_ray,triangle).t;

	if(hit_result.t>1 || hit_result.t<-1 || hit_result.t==0)
	{
		
		return 0;
	}
	else
	{
		return 1;
	}
}

	