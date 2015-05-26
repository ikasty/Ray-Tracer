#ifndef FILE_READ_H
#define FILE_READ_H

#include "include/type.h"

typedef struct {
	int v[3], vn[3], vt[3];
} Triangle;

typedef struct {
	float vect[3];
	float fake_norm[3];
} Vertex;

typedef struct {
	float norm[3];
} Normal;

typedef struct {
	float tex[3];
} Texture;

int file_read(FILE* fp, Data *data, float scale);

#endif
