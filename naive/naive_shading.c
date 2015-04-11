
/*
Copyright (c) 2015, Daeyoun Kang(mail.ikasty@gmail.com),
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

#include "../settings.h"
#include "../include/msl_math.h"
#include "../include/type.h"

unsigned int naive_shading(Ray s_ray, Primitive s_tri, Hit __hit)
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
