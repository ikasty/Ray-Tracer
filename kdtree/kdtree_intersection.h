#ifndef KDTREE_INTERSECTION_H
#define KDTREE_INTERSECTION_H

#include "kdtree_type.h"
#include "../intersection.h"

enum {
	X_AXIS, Y_AXIS, Z_AXIS, LEAF
};

Hit kdtree_intersect_search(Data *data, Ray *ray);

#endif