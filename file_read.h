#ifndef FILE_READ_H
#define FILE_READ_H

#include "include/type.h"

typedef struct {
	int v[3], vn[3];

} Triangle;

typedef struct {
	float vect[3];
	float near_norm[10][3];
	float norm[3];
	int norm_count;
} Vertex;

int file_read(FILE* fp, Data *data, float scale);

#endif
