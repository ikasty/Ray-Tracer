#include <stdio.h>
#include <string.h>
#include "gen_ray.h"

Ray gen_ray(float current_x, float current_y)
{
	Ray make_ray;
	int dir;
	float index[3];

	USE_CAMERA(cam);
	USE_SCREEN(screen);

	index[0] = current_x + cam->orig[0] - screen->xsize / 2;
	index[1] = current_y + cam->orig[1] - screen->ysize / 2;
	index[2] = cam->distance;

	for(dir = 0; dir < 3; dir++)
	{
		make_ray.orig[dir] = cam->orig[dir];
		make_ray.dir[dir] = index[dir] - make_ray.orig[dir];
	}

	make_ray.max_t = MAX_RENDER_DISTANCE;

	return make_ray;
}
