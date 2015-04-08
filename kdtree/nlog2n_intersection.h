#ifndef KDTREE_INTERSECTION_H
#define KDTREE_INTERSECTION_H

#include "kdtree_type.h"
#include "../naive/naive_intersection.h"

enum {
	X_AXIS, Y_AXIS, Z_AXIS, LEAF
};

Hit nlog2n_intersect_search(Data *data, Ray *ray);

#endif