#include "bbox.h"

BBox make_bbox_by_triangle(Primitive tri){
	return union_bbox_and_point(
		make_bbox_by_points(tri.vert0, tri.vert1), tri.vert2
	);
}

BBox make_bbox_by_points(float p1[3], float p2[3]){
	BBox bbox;
	set_backsouthwest(bbox.faaBounds[0], p1, p2);
	set_frontnortheast(bbox.faaBounds[1], p1, p2);

	return bbox;
}

BBox union_bbox_and_point(BBox b, float p[3]){
	BBox bbox;
	set_backsouthwest(bbox.faaBounds[0], b.faaBounds[0], p);
	set_frontnortheast(bbox.faaBounds[1], b.faaBounds[1], p);

	return bbox;
}

BBox union_bbox_and_bbox(BBox b1, BBox b2){
	BBox bbox;
	set_backsouthwest(bbox.faaBounds[0], b1.faaBounds[0], b2.faaBounds[0]);
	set_frontnortheast(bbox.faaBounds[1], b1.faaBounds[1], b2.faaBounds[1]);

	return bbox;
}

void set_backsouthwest(float des[3], float p1[1], float p2[2]){
	des[0] = p1[0]<p2[0]? p1[0]: p2[0];
	des[1] = p1[1]<p2[1]? p1[1]: p2[1];
	des[2] = p1[2]<p2[2]? p1[2]: p2[2];
}
void set_frontnortheast(float des[3], float p1[1], float p2[2]){
	des[0] = p1[0]>p2[0]? p1[0]: p2[0];
	des[1] = p1[1]>p2[1]? p1[1]: p2[1];
	des[2] = p1[2]>p2[2]? p1[2]: p2[2];
}