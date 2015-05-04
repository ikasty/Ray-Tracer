#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "include/type.h"

extern void (*clear_accel)(Data *data);
extern void (*accel_build)(Data *data);
extern Hit (*intersect_search)(Data *data, Ray *ray);
extern void (*normal_shade)(float normal_vector[3], float hit_point[3], Primitive s_tri);

void init_search_algo(char *algo_name);
void init_shading_algo(char *shading_name);

#endif
