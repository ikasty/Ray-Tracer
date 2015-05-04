#include "naive_shading.h"

#include "include/msl_math.h"
#include "include/type.h"
#include "algorithms.h"

void naive_shading(float normal_vector[3], float hit_point[3], Primitive s_tri)
{
	float edge1[3], edge2[3];

	SUB(edge1, s_tri.vert1, hit_point);
	SUB(edge2, s_tri.vert2, hit_point);
	CROSS(normal_vector, edge1, edge2);
}