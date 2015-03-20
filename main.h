#ifndef MAIN_H
#define MAIN_H

#include "type.h"

// file_read.c
int file_read(FILE* fp, struct Data *data);

// gen_ray.c
Ray gen_ray(Camera msl_cam, float current_x, float current_y);

// intersection.c
hit intersect_triangle(Ray ray, TriangleVertex triangle);

// shading.c
unsigned int Shading(Ray s_ray, TriangleVertex s_tri, hit __hit);

#endif
