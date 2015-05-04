#include <stdio.h>
#include <string.h>
#include <math.h>

#include "settings.h"
#include "include/msl_math.h"
#include "include/type.h"
#include "algorithms.h"

void get_interpo_norm(float dest[3], float point[3], Primitive prim)
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

unsigned int advanced_shading(Ray ray_light_to_point, Primitive s_tri, Hit hit, Data *data)
{
	unsigned int out_color = 0;
	float hit_point[3], edge1[3], edge2[3];
	float ray_point_to_light[3], ray_length;
	float normal_vector[3], normal_vector2[3], normal_length;
	float h[3], h_length;
	int axis, result_of_color;
	float ld, ls;
	Ray shadow_ray, viewer_ray;
	Hit shadow_hit;

	USE_LIGHT(light);
	USE_CAMERA(cam);

	if (hit.u >= 0 && hit.v >= 0 && hit.u + hit.v <= 1)
	{
		// 접점을 구함
		for (axis = 0; axis < 3; axis++)
		{
			hit_point[axis] = ray_light_to_point.orig[axis] + (hit.t * ray_light_to_point.dir[axis]);
		}	
		// 접점에서 광원까지의 광선을 구함
		SUB(ray_point_to_light, light, hit_point);
		ray_length = (float)sqrtf( length_sq(ray_point_to_light) );
		scalar_multi(ray_point_to_light, 1 / ray_length);

		if (data->primitives[hit.prim_id].norm0[0] == 0 && 
			data->primitives[hit.prim_id].norm0[1] == 0 &&
			data->primitives[hit.prim_id].norm0[2] == 0)
		{
			SUB(edge1, s_tri.vert1, hit_point);
			SUB(edge2, s_tri.vert2, hit_point);
			CROSS(normal_vector2, edge1, edge2);
			CROSS(normal_vector, edge1, edge2);
			normal_length = (float)sqrtf(length_sq(normal_vector));
			if (normal_length > 0){
				scalar_multi(normal_vector, 1 / normal_length);
			}
		}
		else
		{
			get_interpo_norm(normal_vector, hit_point, data->primitives[hit.prim_id]);
			normal_length = (float)sqrtf( length_sq(normal_vector) );
			if (normal_length > 0)
			{
				scalar_multi(normal_vector, 1 / normal_length);
			}
		}				

		// 노멀 벡터와 광원 사이의 cos값을 구함
		ld = DOT(ray_point_to_light, normal_vector);
		if (ld < 0) ld = 0;

		// 접점에서 광원까지의 광선(보정)과 접점에서 눈까지의 광선 추가
		for (axis = 0; axis < 3; axis++)
		{
			// 표면에서 살짝 떨어진 곳을 광선 출발점으로 잡습니다.
			shadow_ray.orig[axis] = hit_point[axis];
			shadow_ray.dir[axis] = light[axis] - shadow_ray.orig[axis];
			viewer_ray.orig[axis] = hit_point[axis];
			viewer_ray.dir[axis] = cam->orig[axis] - viewer_ray.orig[axis];
		}
		shadow_ray.min_t = 0;
		shadow_ray.max_t = MAX_RENDER_DISTANCE;
		viewer_ray.min_t = 0;
		viewer_ray.max_t = MAX_RENDER_DISTANCE;

		// 반사광 계산
		for (axis = 0; axis < 3; axis++)
		{
			h[axis] = viewer_ray.dir[axis] + shadow_ray.dir[axis];
		}
		h_length = (float)sqrtf( length_sq(h) );
		scalar_multi(h, 1 / h_length);
		ls = DOT(normal_vector, h);
		ls = (ls>0) ? ls : 0;
		ls = powf(ls, SHINESS);

		// 그림자 테스트 및 반짝이는 효과 추가
		// 자신의 면에 부딫히는건 판단하지 않음
		shadow_hit = (*intersect_search)(data, &shadow_ray);
		if ((shadow_hit.t > 0) && (shadow_hit.u >= 0) && (shadow_hit.v >= 0) &&
			(shadow_hit.u + shadow_hit.v <= 1) &&
			(shadow_hit.prim_id != hit.prim_id))
		{
			result_of_color = 0;
		}
		else
		{
			result_of_color = (int)(255 * (ld + ls*0.5));
		}
		result_of_color = (result_of_color + 25 > 255) ? 255 : (result_of_color + 25);
		out_color = 0xff000000 | result_of_color << 16 | result_of_color << 8 | result_of_color;
	}
	return out_color;
}
