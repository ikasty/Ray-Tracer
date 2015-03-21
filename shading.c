#include <stdio.h>
#include <string.h>
#include <math.h>
#include "msl_math.h"
#include "type.h"
#include "settings.h"

unsigned int Shading(Ray s_ray, TriangleVertex s_tri, Hit __hit)
{
	unsigned int Out_color = 0;
	float ist_point[3],Ray[3],ortho_line[3],edge1[3],edge2[3],Line_B[3],multi_A[3],multi_B[3];
	int get_ist_point;
	int result_of_color;
	float dot_AB,abs_A,abs_B,multi_AB,inv_multi_AB,cos_AB;
	float tuv[3], temp_ori[3];
	tuv[0] =__hit.t;
	tuv[1] =__hit.u;
	tuv[2] =__hit.v;

	if(tuv[1]>0 && tuv[2]> 0 && tuv[1]+tuv[2] <=1 ){
	
		for(get_ist_point=0; get_ist_point<3; get_ist_point++){
			ist_point[get_ist_point]=s_ray.orig[get_ist_point]+(tuv[0]*s_ray.dir[get_ist_point]);
		}
		//광원 정보를 temp_ori에 적용
		temp_ori[0] = LIGHT_POS_X;
		temp_ori[1] = LIGHT_POS_Y;
		temp_ori[2] = LIGHT_POS_Z;
		SUB(Ray,ist_point,temp_ori);
	
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
	
		result_of_color = (int)(255 * ( (cos_AB-90)/(180-90)>0? (cos_AB-90)/(180-90): 0 ));
		Out_color = 0xff000000 | result_of_color << 16 | result_of_color << 8 | result_of_color;
	}

	return Out_color;
}
