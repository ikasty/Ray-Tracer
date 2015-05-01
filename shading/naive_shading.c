#include <stdio.h>
#include <string.h>
#include <math.h>

#include "../settings.h"
#include "../include/msl_math.h"
#include "../include/type.h"

unsigned int naive_shading(Ray s_ray, Primitive s_tri, Hit __hit, Data *data)
{
	unsigned int Out_color = 0;
	//float Line_B[3];
	float ist_point[3],Ray[3],ortho_line[3],edge1[3],edge2[3],multi_A[3],multi_B[3];
	int get_ist_point;
	int result_of_color;
	//float inv_multi_AB;
	float dot_AB,abs_A,abs_B,multi_AB,cos_AB;
	float tuv[3], temp_ori[3];
	// 광원 설정값을 사용함
	USE_LIGHT(light);

	tuv[0] =__hit.t;
	tuv[1] =__hit.u;
	tuv[2] =__hit.v;	

	if(tuv[1]>0 && tuv[2]> 0 && tuv[1]+tuv[2] <=1 ){
	
		for(get_ist_point=0; get_ist_point<3; get_ist_point++){
			ist_point[get_ist_point]=s_ray.orig[get_ist_point]+(tuv[0]*s_ray.dir[get_ist_point]);
		}
		//광원 정보를 temp_ori에 적용
		temp_ori[0] = light[0];
		temp_ori[1] = light[1];
		temp_ori[2] = light[2];
		SUB(Ray,ist_point,temp_ori);
	
		//면의 노멀 벡터를 구함
		SUB(edge1,s_tri.vert1,ist_point);
		SUB(edge2,s_tri.vert2,ist_point);
		CROSS(ortho_line,edge1,edge2);
		

		dot_AB = DOT(Ray, ortho_line);
		multi_itself(multi_A,Ray);
		multi_itself(multi_B,ortho_line);
		abs_A=(float)sqrtf(abs_line(multi_A));
		abs_B=(float)sqrtf(abs_line(multi_B));
		multi_AB = abs_A * abs_B;
		cos_AB = dot_AB/multi_AB;

		if(cos_AB>1) cos_AB = 1.0;
		else if(cos_AB<-1) cos_AB = -1.0;
		cos_AB=(float)(acosf(cos_AB)*180/PI);
	
		result_of_color = (int)(255 * ( (cos_AB-15)/(180-15)>0? (cos_AB-15)/(180-15): 0 ));
		Out_color = 0xff000000 | result_of_color << 16 | result_of_color << 8 | result_of_color;
	}

	return Out_color;
}
