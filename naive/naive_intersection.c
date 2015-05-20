
/*
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

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "naive_intersection.h"

#include "timecheck.h"
#include "obj_transform.h"
#include "settings.h"
#include "include/msl_math.h"

Hit naive_intersect_search(Data *data, Ray *f_ray)
{
	const int prim_count = data->prim_count;
	Hit min_hit;
	int prim_id;

	USE_TIMECHECK();

	memset(&min_hit, 0, sizeof(min_hit));

	// 각각의 triangle과 f_ray 광선의 교차 검사를 수행함
	// TODO: 교차 검사 수행을 줄일 알고리즘을 도입할 것
	for (prim_id = 0; prim_id < prim_count; prim_id++)
	{
		Hit ist_hit;

		TIMECHECK_START();
		ist_hit = intersect_triangle(f_ray, data->primitives[prim_id]);
		TIMECHECK_END(intersect_clock);

		if (ist_hit.t > 0 && (ist_hit.t<min_hit.t || min_hit.t == 0))
		{
			memcpy(&min_hit, &ist_hit, sizeof(ist_hit));
			min_hit.prim_id = prim_id;
		}
	}

	return min_hit;
}

Hit intersect_triangle(Ray *ray, Primitive prim)
{
	float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
	float det,inv_det;
	static float t, u, v;
	Hit ist_hit;
	memset(&ist_hit, 0, sizeof(ist_hit));

	// ray.orig + t * ray.dir = (1 - u - v) * vert[0] + u * vert[1] + v * vert[2]

	// 점 vert0을 공유하고 있는 삼각형의 두 vector를 구한다
	SUB(edge1, prim.vert[1], prim.vert[0]);
	SUB(edge2, prim.vert[2], prim.vert[0]);
	
	CROSS(pvec, ray->dir, edge2);

	det = DOT(edge1, pvec);

	// EPSILON보다 작으면 = ray가 평면에 너무 붙어있으면
	if (det < EPSILON)
		return ist_hit;

	inv_det = 1.0f / det;

	// ray의 원점에서 점 vert0까지의 거리를 구한다
	SUB(tvec, ray->orig, prim.vert[0]);

	u = DOT(tvec, pvec) * inv_det;
	if (u < 0.0 || u > 1.0)
		return ist_hit;

	CROSS(qvec, tvec, edge1);

	v = DOT(ray->dir, qvec) * inv_det;
	if (v < 0.0 || (u + v) > 1.0)
		return ist_hit;

	t = DOT(edge2, qvec) * inv_det;
	if (t > ray->max_t || t <= 0 || t < ray->min_t)
		return ist_hit;

	ist_hit.t = ray->max_t = t;
	ist_hit.u = u;
	ist_hit.v = v;
	ist_hit.prim_id = prim.prim_id;
	
	return ist_hit;
}
