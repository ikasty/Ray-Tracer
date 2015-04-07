#ifndef FILE_READ_H
#define FILE_READ_H

#include "include/type.h"

typedef struct  {
	int v1;
	int v2;
	int v3;

} Triangle;

typedef struct  {
	float x;
	float y;
	float z;

} Vertex;

int file_read(FILE* fp, Data *data);

#endif
