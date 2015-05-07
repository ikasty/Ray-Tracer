
/*
Original source code from pbrt (https://github.com/mmp/pbrt-v2)

Copyright (c) 1998-2012 Matt Pharr and Greg Humphreys.
Copyright (c) 2015, Minwoo Lee(hellomw@msl.yonsei.ac.kr)
					Daeyoun Kang(mail.ikasty@gmail.com),
					HyungKwan Park(rpdladps@gmail.com),
					Ingyu Kim(goracom0@gmail.com),
					Jungmin Kim(kukakhan@gmail.com)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies, 
either expressed or implied, of the FreeBSD Project.
*/

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