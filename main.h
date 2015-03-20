#ifndef MAIN_H
#define MAIN_H

#include "type.h"

// file_read.c
int file_read(FILE* fp, struct Data *data);

// shading.c
unsigned int Shading(msl_ray s_ray, triangle s_tri, hit __hit);

#endif
