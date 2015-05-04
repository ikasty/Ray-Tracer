#include <string.h>
#include <math.h>

#include "settings.h"
#include "include/msl_math.h"
#include "include/type.h"
#include "algorithms.h"

void phong_shading(float dest[3], float point[3], Primitive prim)
{
	float na[3], nb[3];
	int axis;
	float *p1 = NULL, *p2 = NULL, *p3 = NULL;
	float *n1 = NULL, *n2 = NULL, *n3 = NULL;

	float xs = point[0];
	float ys = point[1];
	float xa, xb;
	
	// 결과 normal 초기화
	for (axis = 0; axis < 3; axis++)
		dest[axis] = 0;

	// 꼭지점 위에 위치했다면 그냥 해당 꼭지점의 normal 반환
	if (is_two_point_equal(point, prim.vert0))
	{
		SUBST(dest, prim.norm0);
		return;
	}
	if (is_two_point_equal(point, prim.vert1))
	{
		SUBST(dest, prim.norm1);
		return;
	}
	if (is_two_point_equal(point, prim.vert2))
	{
		SUBST(dest, prim.norm2);
		return;
	}
	
	// 그 외의 경우 다양한 조건을 고려하여 점 선정
	if ((prim.vert0[1] - point[1])*(point[1] - prim.vert1[1]) >= 0 && prim.vert0[1] != prim.vert1[1]) 
	{
		if ((prim.vert0[1] - point[1]) * (point[1] - prim.vert2[1]) >= 0 &&
			(prim.vert0[1] != prim.vert2[1]) &&
			(prim.vert0[1] != point[1]))
		{
			p1 = prim.vert0;
			n1 = prim.norm0;
			p2 = prim.vert1;
			n2 = prim.norm1;
		}
		else if ((prim.vert1[1] - point[1]) * (point[1] - prim.vert2[1]) >= 0 &&
			(prim.vert1[1] != prim.vert2[1]) &&
			(prim.vert1[1] != point[1]))
		{
			p1 = prim.vert1;
			n1 = prim.norm1;
			p2 = prim.vert0;
			n2 = prim.norm0;
		}
		p3 = prim.vert2;
		n3 = prim.norm2;
	}
	else if ((prim.vert2[1] != prim.vert0[1]) &&
		(prim.vert2[1] != prim.vert1[1]) &&
		(prim.vert2[1] != point[1]))
	{
		p1 = prim.vert2;
		n1 = prim.norm2;
		p2 = prim.vert0;
		n2 = prim.norm0;
		p3 = prim.vert1;
		n3 = prim.norm1;
	}

	// 적당한 점을 선정하지 못할 경우 그냥 return
	if (p1 == NULL || p2 == NULL || p3 == NULL)
		return;
	if (p1[1] == p2[1] || p1[1] == p3[1])
		return;
	
	xa = (p1[0] - p2[0])*(ys - p2[1]) / (p1[1] - p2[1]) + p2[0];
	xb = (p1[0] - p3[0])*(ys - p3[1]) / (p1[1] - p3[1]) + p3[0];
	if (xa == xb)
		return;

	for (axis = 0; axis < 3; axis++)
	{
		na[axis] = (n1[axis] * (ys - p2[1]) + n2[axis] * (p1[1] - ys)) / (p1[1] - p2[1]);
		nb[axis] = (n1[axis] * (ys - p3[1]) + n3[axis] * (p1[1] - ys)) / (p1[1] - p3[1]);
		dest[axis] = (na[axis] * (xs - xb) + nb[axis] * (xa - xs)) / (xa - xb);
	}
}
