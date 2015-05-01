#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../settings.h"
#include "../include/msl_math.h"
#include "../include/type.h"
#include "../kdtree/nlog2n_intersection.h"

unsigned int nlog2n_shading(Ray s_ray, Primitive s_tri, Hit __hit, Data *data)
{
	unsigned int Out_color = 0;
	//float Line_B[3];
	float hit_point[3], ray[3], normal_vector[3], edge1[3], edge2[3], multi_A[3], multi_B[3];
	int axis;
	int result_of_color;
	//float inv_multi_AB;
	float dot_AB, abs_A, abs_B, multi_AB, cos_AB;
	float tuv[3], temp_ori[3];
	Ray shadow_ray;
	Hit shadow_hit;

	// 광원 설정값을 사용함
	USE_LIGHT(light);

	tuv[0] = __hit.t;
	tuv[1] = __hit.u;
	tuv[2] = __hit.v;

	if (tuv[1]>0 && tuv[2]> 0 && tuv[1] + tuv[2] <= 1){
		// 접점을 구함
		for (axis = 0; axis<3; axis++){
			hit_point[axis] = s_ray.orig[axis] + (tuv[0] * s_ray.dir[axis]);			
		}	
		//광원 정보를 temp_ori에 적용 및 접점에서 광원까지의 광선을 구함
		temp_ori[0] = light[0];
		temp_ori[1] = light[1];
		temp_ori[2] = light[2];
		SUB(ray, temp_ori, hit_point);

		//면의 노멀 벡터를 구함
		SUB(edge1, s_tri.vert1, hit_point);
		SUB(edge2, s_tri.vert2, hit_point);
		CROSS(normal_vector, edge1, edge2);

		// 면의 노벌 벡터와 광원 사이의 cos값을 구함
		dot_AB = DOT(ray, normal_vector);
		multi_itself(multi_A, ray);
		multi_itself(multi_B, normal_vector);
		abs_A = (float)sqrtf(abs_line(multi_A));
		abs_B = (float)sqrtf(abs_line(multi_B));
		multi_AB = abs_A * abs_B;
		cos_AB = dot_AB / multi_AB;
		if (cos_AB < 0) cos_AB = 0;

		for (axis = 0; axis < 3; axis++){
			shadow_ray.orig[axis] = hit_point[axis]+normal_vector[axis]/abs_B*0.001f;
			shadow_ray.dir[axis] = light[axis] - shadow_ray.orig[axis];
		}
		shadow_ray.min_t = 0;
		shadow_ray.max_t = MAX_RENDER_DISTANCE;

		// 그림자 테스트
		shadow_hit = nlog2n_intersect_search(data, &shadow_ray);
		if (shadow_hit.t > 0 && shadow_hit.u >= 0 && shadow_hit.v >= 0 && shadow_hit.u + shadow_hit.v <= 1) {
			result_of_color = 0;
		}
		else{
			result_of_color = (int)(255 * (cos_AB));
		}
		result_of_color = result_of_color + 25>255 ? 255 : result_of_color + 25;
		Out_color = 0xff000000 | result_of_color << 16 | result_of_color << 8 | result_of_color;
	}

	return Out_color;
}
