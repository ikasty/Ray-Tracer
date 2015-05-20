
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

#define EPSILON 0.000001f
#define CROSS(dest,v1,v2) {						\
	(dest)[0]=(v1)[1]*(v2)[2]-(v1)[2]*(v2)[1];	\
	(dest)[1]=(v1)[2]*(v2)[0]-(v1)[0]*(v2)[2];	\
	(dest)[2]=(v1)[0]*(v2)[1]-(v1)[1]*(v2)[0];	}

#define DOT(v1,v2) ((v1)[0]*(v2)[0]+(v1)[1]*(v2)[1]+(v1)[2]*(v2)[2])
#define SUB(dest,v1,v2) {		\
	(dest)[0]=(v1)[0]-(v2)[0];	\
	(dest)[1]=(v1)[1]-(v2)[1];	\
	(dest)[2]=(v1)[2]-(v2)[2];	}

#define ADD(dest,v1,v2) {		\
	(dest)[0]=(v1)[0]+(v2)[0];	\
	(dest)[1]=(v1)[1]+(v2)[1];	\
	(dest)[2]=(v1)[2]+(v2)[2];	}

#define multi_itself(x1,x2) {	\
	(x1)[0]=(x2)[0]*(x2)[0];	\
	(x1)[1]=(x2)[1]*(x2)[1];	\
	(x1)[2]=(x2)[2]*(x2)[2];	}
 
#define abs_line(x1) ((x1)[0]+(x1)[1]+(x1)[2])

#define length_sq(x1)	(((x1)[0]*(x1)[0]) + ((x1)[1]*(x1)[1]) + ((x1)[2]*(x1)[2]))
#define scalar_multi(v, sc) {\
	(v)[0] = ((v)[0] * sc); \
	(v)[1] = ((v)[1] * sc); \
	(v)[2] = ((v)[2] * sc); }

#define COPYTO(dest, src){\
	(dest)[0]=(src)[0];\
	(dest)[1]=(src)[1];\
	(dest)[2]=(src)[2];}

#define is_two_point_equal(p1, p2) ((p1)[0] == (p2)[0] && (p1)[1] == (p2)[1] && (p1)[2] == (p2)[2])
	
#define VECTOR_NORMALIZE(vector) {								\
	float ray_length = (float)sqrtf(length_sq(vector));			\
	if (ray_length > 0) scalar_multi(vector, 1 / ray_length);	}

#define PI 3.14159265f

#define next_axis(axis) ((axis + 1) % 3)
