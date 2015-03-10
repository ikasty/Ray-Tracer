 #include <stdio.h>
#include <string.h>
#include <math.h>
#include "msl_math.h"
#include "type.h"

unsigned int Shading(msl_ray s_ray, triangle s_tri, hit __hit)
{
	unsigned int Out_color;
	float ist_point[3],Ray[3],ortho_line[3],edge1[3],edge2[3],Line_B[3],multi_A[3],multi_B[3];
	int get_ist_point;
	int result_of_color;
	float dot_AB,abs_A,abs_B,multi_AB,inv_multi_AB,cos_AB;
	float tuv[3];
	tuv[0] =__hit.t;
	tuv[1] =__hit.u;
	tuv[2] =__hit.v;


#ifdef UV_SHADING
	Out_color =0xff000000 | ((int)(255*ou))<<8 |(int)(255*ov) ;//ARGB
#else
	if(tuv[1]>0 || tuv[2]> 0 || tuv[1]+tuv[2] <=1 )
	{
	
	for(get_ist_point=0;get_ist_point<3;get_ist_point++)
	{
		ist_point[get_ist_point]=s_ray.orig[get_ist_point]+(tuv[0]*s_ray.dir[get_ist_point]);
	}
	//레이와 평면과 교차점에서 수직으로 만나는 수선사이의 각θ에 대한 코사인θ값
	SUB(Ray,ist_point,s_ray.orig);
	SUB(edge1,s_tri.vert1,ist_point);
	SUB(edge2,s_tri.vert2,ist_point);
	CROSS(ortho_line,edge1,edge2);
	SUB(Line_B,ist_point,ortho_line);
	dot_AB=DOT(Ray,Line_B);
	multi_itself(multi_A,Ray);
	multi_itself(multi_B,Line_B);
	abs_A=sqrt(abs_line(multi_A));
	abs_B=sqrt(abs_line(multi_B));
	multi_AB=abs_A * abs_B;
	inv_multi_AB = pow(multi_AB,-1);
	cos_AB=dot_AB * inv_multi_AB;
	if(cos_AB>1)
	{
		cos_AB = 1.0;
	}
	cos_AB=abs(acos(cos_AB)*180/PI);
	result_of_color =255*(cos_AB/90);
	Out_color =0xff000000 | ((int)(result_of_color*tuv[2]))<<8 | ((int)(result_of_color*tuv[1])) ;//ARGB
	
	}
	
#endif
	return Out_color;
}