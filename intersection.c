#include <stdio.h>
#include <string.h>
#include <math.h>
#include "intersection.h"

#include "obj_transform.h"
#include "settings.h"
#include "include/msl_math.h"

Hit intersect_search(Data *data, Ray *f_ray, float index_x, float index_y)
{
	const int triangleCount = data->face_count;
	Hit min_hit;
	int triangle_id;

	memset(&min_hit, 0, sizeof(min_hit));

	// 각각의 triangle과 f_ray 광선의 교차 검사를 수행함
	// TODO: 교차 검사 수행을 줄일 알고리즘을 도입할 것
	for (triangle_id = 0; triangle_id < triangleCount; triangle_id++)
	{
		Hit ist_hit;
		ist_hit = intersect_triangle(f_ray, getTriangle(data->vert, data->face, triangle_id));

		if (ist_hit.t > 0)
		{
			memcpy(&min_hit, &ist_hit, sizeof(ist_hit));
			min_hit.triangle_id = triangle_id;
		}
	}

	return min_hit;
}

Hit intersect_triangle(Ray *ray, TriangleVertex triangle)
{
	float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
	float det,inv_det;
	static float t, u, v;
	Hit ist_hit;
	memset(&ist_hit, 0, sizeof(ist_hit));

	// ray.orig + t * ray.dir = (1 - u - v) * vert0 + u * vert1 + v * vert2

	// 점 vert0을 공유하고 있는 삼각형의 두 vector를 구한다
	SUB(edge1, triangle.vert1, triangle.vert0);
	SUB(edge2, triangle.vert2, triangle.vert0);
	
	CROSS(pvec, ray->dir, edge2);

	det = DOT(edge1, pvec);

	// EPSILON보다 작으면 = ray가 평면에 너무 붙어있으면
	if (det < EPSILON)
		return ist_hit;

	inv_det = 1.0f / det;

	// ray의 원점에서 점 vert0까지의 거리를 구한다
	SUB(tvec, ray->orig, triangle.vert0);

	u = DOT(tvec, pvec) * inv_det;
	if (u < 0.0 || u > 1.0)
		return ist_hit;

	CROSS(qvec, tvec, edge1);

	v = DOT(ray->dir, qvec) * inv_det;
	if (v < 0.0 || (u + v) > 1.0)
		return ist_hit;

	t = DOT(edge2, qvec) * inv_det;
	if (t > ray->max_t)
		return ist_hit;

	ist_hit.t = ray->max_t = t;
	ist_hit.u = u;
	ist_hit.v = v;
	
	return ist_hit;
}
