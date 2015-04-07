#include "bbox.h"

BBox make_bbox_by_triangle(Primitive tri)
{
	return union_bbox_and_point(
		make_bbox_by_points(tri.vert0, tri.vert1), tri.vert2
	);
}

BBox make_bbox_by_points(float p1[3], float p2[3])
{
	BBox bbox;
	set_backsouthwest(bbox.faaBounds[0], p1, p2);
	set_frontnortheast(bbox.faaBounds[1], p1, p2);

	return bbox;
}

BBox union_bbox_and_point(BBox b, float p[3])
{
	BBox bbox;
	set_backsouthwest(bbox.faaBounds[0], b.faaBounds[0], p);
	set_frontnortheast(bbox.faaBounds[1], b.faaBounds[1], p);

	return bbox;
}

BBox union_bbox_and_bbox(BBox b1, BBox b2)
{
	BBox bbox;
	set_backsouthwest(bbox.faaBounds[0], b1.faaBounds[0], b2.faaBounds[0]);
	set_frontnortheast(bbox.faaBounds[1], b1.faaBounds[1], b2.faaBounds[1]);

	return bbox;
}

float get_surface_of_bbox(BBox* b)
{
	float len1 = b->faaBounds[1][0] - b->faaBounds[0][0];
	float len2 = b->faaBounds[1][1] - b->faaBounds[0][1];
	float len3 = b->faaBounds[1][2] - b->faaBounds[0][2];
	return 2 * (len1*len2 + len2*len3 + len3*len1);
}

void set_backsouthwest(float des[3], float p1[1], float p2[2])
{
	des[0] = p1[0]<p2[0]? p1[0]: p2[0];
	des[1] = p1[1]<p2[1]? p1[1]: p2[1];
	des[2] = p1[2]<p2[2]? p1[2]: p2[2];
}

void set_frontnortheast(float des[3], float p1[1], float p2[2])
{
	des[0] = (p1[0] > p2[0]) ? p1[0] : p2[0];
	des[1] = (p1[1] > p2[1]) ? p1[1] : p2[1];
	des[2] = (p1[2] > p2[2]) ? p1[2] : p2[2];
}

// bbox를 각 축에 projection 했을 때 가장 긴 범위를 가진 축을 반환
int find_axis_of_maximum_extent(BBox *b)
{
	float len1 = b->faaBounds[1][0] - b->faaBounds[0][0];
	float len2 = b->faaBounds[1][1] - b->faaBounds[0][1];
	float len3 = b->faaBounds[1][2] - b->faaBounds[0][2];

	if		(len1 > len2 && len1 > len3)	return 0;
	else if	(len2 > len3)					return 1;
	else									return 2;
}