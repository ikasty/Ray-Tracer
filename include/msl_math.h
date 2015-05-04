#include <stdio.h>
#include <string.h>
#include <math.h>

#define EPSILON 0.000001f
#define CROSS(dest,v1,v2) \
	(dest)[0]=(v1)[1]*(v2)[2]-(v1)[2]*(v2)[1]; \
	(dest)[1]=(v1)[2]*(v2)[0]-(v1)[0]*(v2)[2];\
	(dest)[2]=(v1)[0]*(v2)[1]-(v1)[1]*(v2)[0];
				
#define DOT(v1,v2) ((v1)[0]*(v2)[0]+(v1)[1]*(v2)[1]+(v1)[2]*(v2)[2])
#define SUB(dest,v1,v2) \
	(dest)[0]=(v1)[0]-(v2)[0]; \
	(dest)[1]=(v1)[1]-(v2)[1]; \
	(dest)[2]=(v1)[2]-(v2)[2];

#define ADD(dest,v1,v2) \
	(dest)[0]=(v1)[0]+(v2)[0]; \
	(dest)[1]=(v1)[1]+(v2)[1]; \
	(dest)[2]=(v1)[2]+(v2)[2];

#define multi_itself(x1,x2)\
	(x1)[0]=(x2)[0]*(x2)[0];\
	(x1)[1]=(x2)[1]*(x2)[1]; \
	(x1)[2]=(x2)[2]*(x2)[2]; 
#define abs_line(x1) ((x1)[0]+(x1)[1]+(x1)[2])

#define length_sq(x1)	(((x1)[0]*(x1)[0]) + ((x1)[1]*(x1)[1]) + ((x1)[2]*(x1)[2]))
#define scalar_multi(v, sc)\
	(v)[0] = ((v)[0] * sc);\
	(v)[1] = ((v)[1] * sc);\
	(v)[2] = ((v)[2] * sc);

#define COPYTO(dest, src){\
	(dest)[0]=(src)[0];\
	(dest)[1]=(src)[1];\
	(dest)[2]=(src)[2];}

#define is_two_point_equal(p1, p2) ((p1)[0] == (p2)[0] && (p1)[1] == (p2)[1] && (p1)[2] == (p2)[2])
	
#define VECTOR_NORMALIZE(vector) {								\
	float ray_length = (float)sqrtf(length_sq(vector));			\
	if (ray_length > 0) scalar_multi(vector, 1 / ray_length);	}

#define PI 3.14159265f

#define next_axis(axis) ((axis + 1) % 3)
