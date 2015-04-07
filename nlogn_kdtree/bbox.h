#ifndef __BBOX_H__
#define __BBOX_H__

#include "kdtree_type.h"

BBox make_bbox_by_triangle(Primitive tri);
BBox make_bbox_by_points(float p1[3], float p2[3]);
BBox union_bbox_and_point(BBox b, float p[3]);
BBox union_bbox_and_bbox(BBox b1, BBox b2);

void set_backsouthwest(float des[3], float p1[1], float p2[2]);
void set_frontnortheast(float des[3], float p1[1], float p2[2]);

float get_surface_of_bbox(BBox* b);
int find_axis_of_maximum_extent(BBox *b);

#endif