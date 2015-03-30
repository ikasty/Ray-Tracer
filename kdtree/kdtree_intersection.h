#ifndef KDTREE_INTERSECTION_H
#define KDTREE_INTERSECTION_H

#include "kdtree_type.h"

enum {
	X_AXIS, Y_AXIS, Z_AXIS, LEAF
};

Hit kdtree_intersect_search(Data *data, Ray *ray);
static bool box_IntersectP(BBox b_box, Ray ray, float *hit_t0, float *hit_t1);

#endif