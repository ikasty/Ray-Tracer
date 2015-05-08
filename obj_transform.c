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
static float transformation[3][3];

/* 
set_rotate: 받은 frame 번호를 바탕으로 로테이션에 필요한 기본 정보를 채우는 함수입니다.
*/
void set_rotate(int num_frame)
{
	float degree, radian;
	float r_cos, r_sin;

	USE_SCREEN(screen);

	// frame 개수만큼 돌아갈 각도를 계산합니다.
	degree = 360.f / screen->frame_count;

	// 각도 표현을 라디안 표현법으로 고치기
	radian = degree * (float)PI / 180; 

	r_cos = (float)cosf(radian);
	r_sin = (float)sinf(radian);

	// transformation 배열의 모든 값을 0으로 초기화 합니다.
	memset(transformation, 0, sizeof(transformation));

	// transformation 수행에 필요한 행렬에 값을 집어넣습니다.
	transformation[0][0] =  r_cos;
	transformation[0][2] =  r_sin;
	transformation[1][1] =  1;
	transformation[2][0] = -r_sin;
	transformation[2][2] =  r_cos;

	// x축 회전
	/*
	transformation[0][0] =  1;
	transformation[1][1] =  r_cos;
	transformation[1][2] = -r_sin;
	transformation[2][1] =  r_sin;
	transformation[2][2] =  r_cos;
	*/
	// z축 회전
	/*
	transformation[0][0] =  r_cos;
	transformation[0][1] = -r_sin;
	transformation[1][0] =  r_sin;
	transformation[1][1] =  r_cos;
	transformation[2][2] =  1;
	*/
}

/* 
get_rotated_vector: vector를 회전시키는 함수입니다. 
- rotated_vector: 회전 결과가 들어갈 vector입니다.
*/
void get_rotated_vector(float *rotated_vector)
{
	int i, j;
	float original_vector[3];

	memcpy(original_vector, rotated_vector, sizeof(original_vector));

	// transformation을 수행하여 원래 좌표에서 회전 후 좌표값을 찾습니다.
	for (i = 0; i < 3; i++)
	{
		rotated_vector[i] = 0;
		for (j = 0; j < 3; j++)
		{
			rotated_vector[i] += transformation[i][j] * original_vector[j];
		}
	}

	return ;
}
