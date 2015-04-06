#ifndef INTERSECTION_H
#define INTERSECTION_H

#include "include/type.h"

Hit naive_intersect_search(Data *data, Ray *f_ray, float index_x, float index_y);
Hit intersect_triangle(Ray *ray, Primitive triangle);

#endif
