
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

#include "obj_transform.h"

#include "settings.h"
#include "include/type.h"
#include "include/msl_math.h"
#include "include/debug-msg.h"

// 삼각형 로테이트 시키기
// 님들 선대시간에 회전 transformation할때 쓰는 행렬 배웠죠? 이게 그겁니다. 으아아앙!
static float transformation[6][3][3]; // direction, x, y

/* 
set_rotate: 받은 frame 번호를 바탕으로 로테이션에 필요한 기본 정보를 채우는 함수입니다.
*/
void set_rotate(float degree)
{
	float radian;
	float r_cos, r_sin;

	// 각도 표현을 라디안 표현법으로 고치기
	radian = degree * (float)PI / 180; 

	r_cos = (float)cosf(radian);
	r_sin = (float)sinf(radian);

	// transformation 배열의 모든 값을 0으로 초기화 합니다.
	memset(transformation, 0, sizeof(transformation));

	// transformation 수행에 필요한 행렬에 값을 집어넣습니다.

	// x축 회전
	transformation[X][0][0] = 1;
	transformation[X][1][1] = r_cos;
	transformation[X][1][2] = -r_sin;
	transformation[X][2][1] = r_sin;
	transformation[X][2][2] = r_cos;

	// y축 회전
	transformation[Y][0][0] =  r_cos;
	transformation[Y][0][2] =  r_sin;
	transformation[Y][1][1] =  1;
	transformation[Y][2][0] = -r_sin;
	transformation[Y][2][2] =  r_cos;

	// z축 회전
	transformation[Z][0][0] =  r_cos;
	transformation[Z][0][1] = -r_sin;
	transformation[Z][1][0] =  r_sin;
	transformation[Z][1][1] =  r_cos;
	transformation[Z][2][2] =  1;

	// x축 회전 (반대방향)
	transformation[RX][0][0] = 1;
	transformation[RX][1][1] = r_cos;
	transformation[RX][1][2] = r_sin;
	transformation[RX][2][1] = -r_sin;
	transformation[RX][2][2] = r_cos;

	// y축 회전 (반대방향)
	transformation[RY][0][0] = r_cos;
	transformation[RY][0][2] = -r_sin;
	transformation[RY][1][1] = 1;
	transformation[RY][2][0] = r_sin;
	transformation[RY][2][2] = r_cos;

	// z축 회전 (반대방향)
	transformation[RZ][0][0] = r_cos;
	transformation[RZ][0][1] = r_sin;
	transformation[RZ][1][0] = -r_sin;
	transformation[RZ][1][1] = r_cos;
	transformation[RZ][2][2] = 1;
}

/* 
get_rotated_vector: vector를 회전시키는 함수입니다. 
- rotated_vector: 회전 결과가 들어갈 vector입니다.
*/
void get_rotated_vector(float *rotated_vector, int direction)
{
	int i, j;
	float original_vector[3];
	
	if (direction < -3 || direction > 3) return;
	if (!direction) return;

	if (direction < 0)
	{
		direction = 3 - direction;
	}
	direction--;

	memcpy(original_vector, rotated_vector, sizeof(original_vector));

	// transformation을 수행하여 원래 좌표에서 회전 후 좌표값을 찾습니다.
	for (i = 0; i < 3; i++)
	{
		rotated_vector[i] = 0;
		for (j = 0; j < 3; j++)
		{
			rotated_vector[i] += transformation[direction][i][j] * original_vector[j];
		}
	}

	return ;
}
