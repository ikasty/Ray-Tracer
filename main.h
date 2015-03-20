#ifndef MAIN_H
#define MAIN_H

// global type definition
#include "type.h"

// file_read.c
//int file_read(FILE* fp, struct Data *data);
#include "file_read.h"

// gen_ray.c
//Ray gen_ray(Camera msl_cam, float current_x, float current_y);
#include "gen_ray.h"

// intersection.c
//Hit intersect_triangle(Ray ray, TriangleVertex triangle);
#include "intersection.h"

// shading.c
//unsigned int Shading(Ray s_ray, TriangleVertex s_tri, Hit __hit);
#include "shading.h"


#endif
