#ifndef KDTREE_INTERSECT_CLEAR_H
#define KDTREE_INTERSECT_CLEAR_H

#include "kdtree_type.h"
#include "naive/naive_intersection.h"

enum {
	X_AXIS, Y_AXIS, Z_AXIS, LEAF
};

Hit kdtree_intersect_search(Data *data, Ray *ray);
void kdtree_clear_accel(Data *data);

#endif