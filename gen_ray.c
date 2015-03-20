#include <stdio.h>
#include <string.h>
#include "type.h"
#include "settings.h"

msl_ray gen_ray(Camera cam, float current_x, float current_y)
{
	msl_ray make_ray;
	int dir;
	float index[3];

	index[0] = current_x + cam.orig[0];
	index[1] = current_y + cam.orig[1];
	index[2] = cam.distance;

	for(dir = 0; dir < 3; dir++)
	{
		make_ray.orig[dir] = cam.orig[dir];
		make_ray.dir[dir] = index[dir] - make_ray.orig[dir];
	}

	make_ray.max_t = MAX_RENDER_DISTANCE;

	return make_ray;
}
