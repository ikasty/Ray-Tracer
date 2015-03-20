#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "type.h"

hit intersect_search(Data *data, msl_ray *f_ray, float index_x, float index_y);
msl_ray gen_ray(camera msl_cam, float current_x, float current_y);
static hit intersect_triangle(msl_ray *ray, triangle triangle);

#endif