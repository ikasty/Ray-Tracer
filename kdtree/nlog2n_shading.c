#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../settings.h"
#include "../include/msl_math.h"
#include "../include/type.h"
#include "../kdtree/nlog2n_intersection.h"

unsigned int nlog2n_shading(Ray ray_light_to_point, Primitive s_tri, Hit hit, Data *data)
{
	unsigned int out_color = 0;
	float hit_point[3], edge1[3], edge2[3];
	float ray_point_to_light[3], ray_length;
	float normal_vector[3], normal_length;
	float h[3], h_length;
	int axis, result_of_color;
	float ld, ls;
	Ray shadow_ray, viewer_ray;
	Hit shadow_hit;

	USE_LIGHT(light);
	USE_CAMERA(cam);

	if (hit.u >= 0 && hit.v >= 0 && hit.u + hit.v <= 1) {
		// 접점을 구함
		for (axis = 0; axis<3; axis++){
			hit_point[axis] = ray_light_to_point.orig[axis] + (hit.t * ray_light_to_point.dir[axis]);
		}	
		// 접점에서 광원까지의 광선을 구함
		SUB(ray_point_to_light, light, hit_point);
		ray_length = (float)sqrtf(length_sq(ray_point_to_light));
		scalar_multi(ray_point_to_light, 1 / ray_length);

		// 노멀 벡터를 구함
		SUB(edge1, s_tri.vert1, hit_point);
		SUB(edge2, s_tri.vert2, hit_point);
		CROSS(normal_vector, edge1, edge2);
		normal_length = (float)sqrtf(length_sq(normal_vector));
		scalar_multi(normal_vector, 1 / normal_length);

		// 노멀 벡터와 광원 사이의 cos값을 구함
		ld = DOT(ray_point_to_light, normal_vector);
		if (ld < 0) ld = 0;

		// 접점에서 광원까지의 광선(보정)과 접점에서 눈까지의 광선 추가
		for (axis = 0; axis < 3; axis++){
			// 표면에서 살짝 떨어진 곳을 광선 출발점으로 잡습니다.
			shadow_ray.orig[axis] = hit_point[axis] + normal_vector[axis]*0.001f;
			shadow_ray.dir[axis] = light[axis] - shadow_ray.orig[axis];
			viewer_ray.orig[axis] = hit_point[axis];
			viewer_ray.dir[axis] = cam->orig[axis] - viewer_ray.orig[axis];
		}
		shadow_ray.min_t = 0;
		shadow_ray.max_t = MAX_RENDER_DISTANCE;
		viewer_ray.min_t = 0;
		viewer_ray.max_t = MAX_RENDER_DISTANCE;

		// 반사광 계산
		for (axis = 0; axis < 3; axis++){
			h[axis] = viewer_ray.dir[axis] + shadow_ray.dir[axis];
		}
		h_length = (float)sqrtf(length_sq(h));
		scalar_multi(h, 1 / h_length);
		ls = DOT(normal_vector, h);
		ls = ls>0 ? ls : 0;
		ls = powf(ls, SHINESS);

		// 그림자 테스트 및 반짝이는 효과 추가
		shadow_hit = nlog2n_intersect_search(data, &shadow_ray);
		if (shadow_hit.t > 0 && shadow_hit.u >= 0 && shadow_hit.v >= 0 && shadow_hit.u + shadow_hit.v <= 1) {
			result_of_color = 0;
		}
		else{
			result_of_color = (int)(255 * (ld + ls*0.2));
		}
		result_of_color = result_of_color + 25 > 255 ? 255 : result_of_color + 25;
		out_color = 0xff000000 | result_of_color << 16 | result_of_color << 8 | result_of_color;
	}
	return out_color;
}
