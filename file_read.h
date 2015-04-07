#ifndef FILE_READ_H
#define FILE_READ_H

#include "include/type.h"

typedef struct {
	int v[3], vn[3];

} Triangle;

typedef struct {
	float vect[3];

} Vertex;

int file_read(FILE* fp, Data *data);

#endif
