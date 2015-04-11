
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
#include "include/type.h"
#include <math.h>

// 임시로 주석 처리함
/*
int shadow_test(Ray ray, float l_source[3],Primitive triangle)
{
	Ray shadow_ray;
	Hit hit_result;
	
//	int exist_shadow;
	float shadow_XYZ[3];
	float shadow_t;
//	float intersect_t;
	int repetition;

	
	shadow_t=-(ray.orig[1]/ray.dir[1]);
	

	//computing shadow's x,y,z coordinate
	shadow_XYZ[0]=ray.orig[0]+(shadow_t * ray.dir[0]);
	shadow_XYZ[1]=0;
	shadow_XYZ[2]=ray.orig[2]+(shadow_t * ray.dir[2]);

	for(repetition=0;repetition<3;repetition++)
	{
		shadow_ray.orig[repetition]=shadow_XYZ[repetition];
		shadow_ray.dir[repetition]=l_source[repetition]-shadow_XYZ[repetition];
	}
	hit_result.t=intersect_triangle(shadow_ray,triangle).t;

	if(hit_result.t>1 || hit_result.t<-1 || hit_result.t==0)
	{
		
		return 0;
	}
	else
	{
		return 1;
	}
}
*/