#include <stdio.h>
#include <string.h>
#include <math.h>

#include "shading.h"

// naive shading은 항상 필요함
#include "naive_shading.h"

#include "settings.h"
#include "include/msl_math.h"
#include "include/type.h"
#include "algorithms.h"

#define VECTOR_NORMALIZE(vector) {								\
	float ray_length = (float)sqrtf(length_sq(vector));			\
	if (ray_length > 0) scalar_multi(vector, 1 / ray_length);	}

unsigned int shading(Ray ray_screen_to_point, Primitive primitive, Hit hit, Data *data)
{
	unsigned int out_color = 0;
	float hit_point[3];
	float ray_point_to_light[3], normal_vector[3];
	float h[3];
	int axis, result_of_color;
	float ld, ls, la;
	Ray shadow_ray, viewer_ray;
	Hit shadow_hit;

	USE_LIGHT(light);

	la = 25;

	if (hit.t > 0)
	{
		// 접점을 구함
		for (axis = 0; axis < 3; axis++)
		{
			hit_point[axis] = ray_screen_to_point.orig[axis] + (hit.t * ray_screen_to_point.dir[axis]);
		}

		// 접점에서 광원까지의 광선을 구함
		SUB(ray_point_to_light, light, hit_point);
		VECTOR_NORMALIZE(ray_point_to_light);

		// 노멀 벡터가 없거나 노멀 벡터 함수가 지정되지 않은 경우
		if (primitive.use_normal == 0 || normal_shade == NULL)
		{
			naive_shading(normal_vector, hit_point, primitive);
		}
		else
		{
			(*normal_shade)(normal_vector, hit_point, primitive);
		}
		VECTOR_NORMALIZE(normal_vector);

		// 노멀 벡터와 광원 사이의 cos값을 구함
		ld = DOT(ray_point_to_light, normal_vector);
		if (ld < 0) ld = 0;

		// 그림자 등에서 사용될 반사광선, 접점에서 스크린까지의 광선 계산
		for (axis = 0; axis < 3; axis++)
		{
			shadow_ray.orig[axis] = hit_point[axis];
			shadow_ray.dir[axis] = ray_point_to_light[axis];
			viewer_ray.orig[axis] = hit_point[axis];
			viewer_ray.dir[axis] = -ray_screen_to_point.dir[axis];
		}
		// 표면에서 살짝 떨어진 곳을 반사광선 출발점으로 잡습니다.
		shadow_ray.min_t = 0.001f;
		shadow_ray.max_t = MAX_RENDER_DISTANCE;

		// 반사광 계산 (Blinn-Phong Illumination Model)
		for (axis = 0; axis < 3; axis++)
		{
			h[axis] = viewer_ray.dir[axis] + shadow_ray.dir[axis];
		}
		VECTOR_NORMALIZE(h);
		
		ls = DOT(normal_vector, h);
		if (ls < 0) ls = 0;

		ls = powf(ls, SHINESS);

		// 그림자 테스트 및 반짝이는 효과 추가
		// 자신의 면에 부딫히는건 판단하지 않음
		shadow_hit = (*intersect_search)(data, &shadow_ray);
		if ((shadow_hit.t > 0) && (shadow_hit.prim_id != hit.prim_id))
		{
			result_of_color = 0;
		}
		else
		{
			result_of_color = (int)(255 * (ld + ls * 0.5));
		}
		result_of_color = result_of_color + la;
		if (result_of_color > 255) result_of_color = 255;

		out_color = 0xff000000 | result_of_color << 16 | result_of_color << 8 | result_of_color;
	}
	return out_color;
}
