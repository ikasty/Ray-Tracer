#ifndef MAIN_H
#define MAIN_H

#include "type.h"

// file_read.c
int file_read(FILE* fp, struct Data *data);

// gen_ray.c
msl_ray gen_ray(camera msl_cam, float current_x, float current_y);

// intersection.c
hit intersect_triangle(msl_ray ray, TriangleVertex triangle);

// shading.c
unsigned int Shading(msl_ray s_ray, TriangleVertex s_tri, hit __hit);

#endif
