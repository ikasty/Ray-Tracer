#include <stdio.h>
#include <string.h>
#include <math.h>
#include "msl_math.h"
#include "type.h"


hit intersect_triangle(msl_ray ray, triangle triangle)
{
	float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
	float det,inv_det;
	static float t, u, v;
	hit ist_hit;
	memset(&ist_hit, 0, sizeof(ist_hit));
	
	// 점 vert0을 공유하고 있는 삼각형의 두 vector를 구한다
	SUB(edge1, triangle.vert1, triangle.vert0);
	SUB(edge2, triangle.vert2, triangle.vert0);
	
	CROSS(pvec, ray.dir, edge2);

	det = DOT(edge1, pvec);

	// EPSILON보다 작으면 = ray가 평면에 너무 붙어있으면
	if (det < EPSILON)
		return ist_hit;

	inv_det = 1.0 / det;

	// ray의 원점에서 점 vert0까지의 거리를 구한다
	SUB(tvec, ray.orig, triangle.vert0);

	u = DOT(tvec, pvec) * inv_det;
	if (u < 0.0 || u > 1.0)
		return ist_hit;

	CROSS(qvec, tvec, edge1);

	v = DOT(ray.dir, qvec) * inv_det;
	if (v < 0.0 || (u + v) > 1.0)
		return ist_hit;

	t = DOT(edge2, qvec) * inv_det;

	ist_hit.t = t;
	ist_hit.u = u;
	ist_hit.v = v;
	
	return ist_hit;
}
