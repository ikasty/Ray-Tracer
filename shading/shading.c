
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
#include "include/debug-msg.h"
#include "algorithms.h"

enum {X, Y, Z};
#define FRACTION(x) ((x) - floor((float)(x)))

void get_pos_on_texture_for_point(int pos[2], float point[3], Primitive prim, Image *img)
{
	float prim_edge[3][3], vert_point[3][3], tex[3][2];
	float inter_t[3][2], inter_p[3][3];
	int inter_count = 0;
	int axis, i, j;

	COPYTO(vert_point[0], prim.vert[0]);
	COPYTO(vert_point[1], prim.vert[1]);
	COPYTO(vert_point[2], prim.vert[2]);
	// 텍스쳐 상의 실제 좌표는 소수부에 의해서 결정됨
	for (i = 0; i < 2; i++){
		tex[0][i] = prim.text[0][i];
		tex[1][i] = prim.text[1][i];
		tex[2][i] = prim.text[2][i];
	}

	// 텍스쳐 좌표를 찾고자하는 점이 prim의 꼭지점이라면, 
	// 해당하는 꼭지점의 텍스쳐 좌표 반환
	for (i = 0; i < 3; i++)
	{
		if (is_two_point_equal(point, vert_point[i]))
		{
			// 텍스쳐 좌표의 범위는 0~width-1, 0~height-1
			pos[0] = max((int)(FRACTION(tex[i][X]) * img->width - 1), 0);
			pos[1] = img->height - max((int)(FRACTION(tex[i][Y]) * img->height - 1), 0);
			return;
		}
	}
	
	for (i = 0; i < 3; i++)
	{
		// 방향 계산
		SUB(prim_edge[i], vert_point[next_axis(i)], vert_point[i]);
	}
	pos[0] = 0;
	pos[1] = 0;

	// xz 평면부터 시도함
	axis = Y;
retry:	
	inter_count = 0;
	// 접점을 지나면서 평면에 수평한 평면과, 각 edge와의 접점을 구함.
	for (i = 0; i < 3; i++)
	{
		float u = -1;

		// P + u*D = inter_p 가 될 수 있도록 u를 구함.
		if (prim_edge[i][axis] != 0)
		{
			u = (point[axis] - vert_point[i][axis]) / prim_edge[i][axis];

			// 교점이 없으면 쓸모가 없다
			if (!(0 <= u && u <= 1)) continue;

			// 적당한 u가 구해졌다면 접점을 확정지음
			for (j = 0; j < 3; j++)
			{
				// 접점 위치
				inter_p[inter_count][j] = vert_point[i][j] + prim_edge[i][j] * u;
			}
			// 접점의 버텍스 좌표값
			inter_t[inter_count][X] = (1 - u) * tex[i][X] + u * tex[next_axis(i)][X];
			inter_t[inter_count][Y] = (1 - u) * tex[i][Y] + u * tex[next_axis(i)][Y];
			inter_count++;
		}
	}

	// 접점이 2개 나와야 정상임
	if (inter_count != 2)
	{
		axis = next_axis(axis);
		if (axis == Y)
		{
			// 세개 평면으로 시도해도 전부 실패함
			PDEBUG("phong_shading.c: cannot find normal vector\n");
			return;
		}
		goto retry;
	}
	else
	{
		float temp[3], temp2[3], temp_length, temp2_length;
		float frac[2];

		// 위에서 구한 두 접점과 점의 거리를 구함
		SUB(temp, inter_p[0], point);
		temp_length = sqrtf(length_sq(temp));
		SUB(temp2, inter_p[1], point);
		temp2_length = sqrtf(length_sq(temp2));

		if (temp_length + temp2_length == 0)
		{
			PDEBUG("phong_shading.c: Unsolved error occured!\n");
		}

		frac[0] = (float)FRACTION((inter_t[0][X] * temp2_length + inter_t[1][X] * temp_length) / (temp_length + temp2_length));
		pos[0] = max((int)(frac[0] * img->width - 1), 0);
		frac[1] = (float)FRACTION((inter_t[0][Y] * temp2_length + inter_t[1][Y] * temp_length) / (temp_length + temp2_length));
		pos[1] = img->height - max((int)(frac[1] * img->height - 1), 0);
	}
}

void get_rgb_for_point(RGBA *color, float point[3], Primitive prim, Data *data){
	int pos[2];

	get_pos_on_texture_for_point(pos, point, prim, &data->texture);
	*color = data->texture.pixels[pos[0]][pos[1]];
}

RGBA shading(Ray ray_screen_to_point, Primitive primitive, Hit hit, Data *data)
{
	float hit_point[3];
	float normal_vector[3], viewer_vector[3];
	float h[3];
	int axis, i;
	unsigned int rgb[3];
	RGBA mat_rgb;
	float ld, ls, la;
	Ray ray_point_to_light;
	Hit shadow_hit;

	USE_LIGHT(light);
	USE_TIMECHECK();

	la = 0.1f;
	mat_rgb.i = 0xffffffff;
	

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

		// texture가 있다면 사용함
		if (primitive.use_texture == 1 && data->texture.pixels != NULL){
			get_rgb_for_point(&mat_rgb, hit_point, primitive, data);
		}		

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
			//COPYTO(rgb, mat_rgb.l+1);
			//scalar_multi(rgb, la);
			for (i = 0; i < 3; i++) {
				rgb[i] = (int)(mat_rgb.l[i] * la);
			}
		}
		else
		{
			//COPYTO(rgb, mat_rgb.l+1);
			//scalar_multi(rgb, ld + ls * 0.5 + la);
			for (i = 0; i < 3; i++) {
				rgb[i] = (int)(mat_rgb.l[i] * (ld + ls * 0.5 + la));
			}
		}

		for(i = 0; i < 3; i++) {
			if(rgb[i] > 255) {
				rgb[i] = 255;
			}
		}
  mat_rgb.i = 0xff000000 | rgb[2] << 16 | rgb[1] << 8 | rgb[0];
  //mat_rgb.i = rgb[0] << 24 | rgb[1] << 16 | rgb[2] << 8 | 0xff;
 }
  //PDEBUG("(0, 1, 2, 3) - (%d,%d,%d,%d)\n", mat_rgb.l[0], mat_rgb.l[1], mat_rgb.l[2], mat_rgb.l[3]);
  //PDEBUG("(r, g, b, a) = (%d,%d,%d,%d)\n", mat_rgb.r, mat_rgb.g, mat_rgb.b, mat_rgb.a);
	return mat_rgb;
}
