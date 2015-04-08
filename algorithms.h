#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "include/type.h"

extern void (*accel_build)(Data *data);
extern Hit (*intersect_search)(Data *data, Ray *ray);
extern unsigned int (*shading)(Ray s_ray, Primitive s_tri, Hit __hit);

void init_search_algo(char *algo_name);

#endif
