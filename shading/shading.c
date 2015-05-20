
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

#include "shading.h"

// naive shading은 항상 필요함
#include "naive_shading.h"

#include "timecheck.h"
#include "settings.h"
#include "include/msl_math.h"
#include "include/type.h"
#include "algorithms.h"

unsigned int shading(Ray ray_screen_to_point, Primitive primitive, Hit hit, Data *data)
{
	unsigned int out_color = 0;
	float hit_point[3];
	float normal_vector[3], viewer_vector[3];
	float h[3];
	int axis, result_of_color;
	float ld, ls, la;
	Ray ray_point_to_light;
	Hit shadow_hit;

	USE_LIGHT(light);
	USE_TIMECHECK();

	la = 25;

	if (hit.t > 0)
	{
		// 접점을 구함
		for (axis = 0; axis < 3; axis++)
		{
			hit_point[axis] = ray_screen_to_point.orig[axis] + (hit.t * ray_screen_to_point.dir[axis]);
			ray_point_to_light.orig[axis] = hit_point[axis];
		}
		SUB(ray_point_to_light.dir, light, hit_point);
		VECTOR_NORMALIZE(ray_point_to_light.dir);
		ray_point_to_light.min_t = 0;
		ray_point_to_light.max_t = MAX_RENDER_DISTANCE;

		// 노멀 벡터 함수가 지정되지 않은 경우
		if (normal_shade == NULL)
		{
			naive_shading(normal_vector, hit_point, primitive);
		}
		else
		{
			(*normal_shade)(normal_vector, hit_point, primitive);
		}
		VECTOR_NORMALIZE(normal_vector);

		// 노멀 벡터와 광원 사이의 cos값을 구함
		ld = DOT(ray_point_to_light.dir, normal_vector);
		if (ld < 0) ld = 0;

		// 접점에서 뷰어로 향하는 벡터 구하기 
		for (axis = 0; axis < 3; axis++)
		{
			viewer_vector[axis] = -ray_screen_to_point.dir[axis];
		}
		VECTOR_NORMALIZE(viewer_vector);

		// 반사광 계산 (Blinn-Phong Illumination Model)
		for (axis = 0; axis < 3; axis++)
		{
			h[axis] = viewer_vector[axis] + ray_point_to_light.dir[axis];
		}
		VECTOR_NORMALIZE(h);
		
		ls = DOT(normal_vector, h);
		if (ls < 0) ls = 0;
		ls = powf(ls, SHINESS);

		// 그림자 테스트 및 반짝이는 효과 추가
		// 자신의 면에 부딫히는건 판단하지 않음
		TIMECHECK_START();
		shadow_hit = (*intersect_search)(data, &ray_point_to_light);
		TIMECHECK_END(shadow_search_clock);

		if ((shadow_hit.t > 0) && (shadow_hit.prim_id != hit.prim_id))
		{
			result_of_color = 0;
		}
		else
		{
			result_of_color = (int)(255 * (ld + ls * 0.5));
		}
		result_of_color = result_of_color + (int)la;
		if (result_of_color > 255) result_of_color = 255;

		out_color = 0xff000000 | result_of_color << 16 | result_of_color << 8 | result_of_color;
	}
	return out_color;
}
