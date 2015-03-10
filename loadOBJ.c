#include <stdio.h>
#include <string.h>
#include "type.h"
#include "msl_math.h"
#include <math.h>

/* 
코드 이름에 속지 마세요. 
전반적으로 각도 변화에 따라 삼격형의 좌표를 바꿔주는 것에 대한 정보가 들어가 있습니다.
*/

// 삼각형 로테이트 시키기
// 님들 선대시간에 회전 transformation할때 쓰는 행렬 배웠죠?
// 이게 그겁니다. 으아아앙!
float transformation[3][3];

/* 
set_rotate: 받은 frame 번호를 바탕으로 로테이션에 필요한 기본 정보를 채우는 함수입니다.
- num_frame: 몇번째 frame인지를 나타내는 값입니다.
*/
void set_rotate(int num_frame){
	float degree,radian;
	float r_cos, r_sin;

	// frame이 30개이므로 한 frame당 12도씩 돌아가게 만들어져 있습니다.
	degree = 12.0;
	// 각도 표현을 라디안 표현법으로 고치기
	radian=degree*num_frame*PI/180; 
	
	r_cos=cos(radian);
	r_sin=sin(radian);
	
	// transformation 배열의 모든 값을 0으로 초기화 합니다.
	memset(transformation, 0 ,sizeof(transformation));
	// transformation 수행에 필요한 행렬에 값을 집어넣습니다.
	transformation[0][0]=1;
	transformation[1][1]=r_cos;
	transformation[1][2]=-r_sin;
	transformation[2][1]=r_sin;
	transformation[2][2]=r_cos;
}

/* 
get_rotated_vector: vector를 회전시키는 함수입니다. 
- theta: 원래 각도가 들어갔어야 될것 같지만 이제는 아무 상관 없엉
- original_vector: 회전을 시키고 싶은 vector입니다.
- rotated_vector: 회전 결과가 들어갈 vector입니다.
*/
void get_rotated_vector(float theta, float *original_vector, float *rotated_vector)
{
	int i,j;

	// transformation을 수행하여 원래 좌표에서 회전 후 좌표값을 찾습니다.
	for(i=0; i<3; i++)
	{
		rotated_vector[i] = 0;
		for(j=0; j<3; j++)
		{
			rotated_vector[i] += transformation[i][j]*original_vector[j];                             
		}
	}
	return;
}

/* 
getTriangle: 회전을 수행한 후 삼각형의 좌표를 반환하는 함수입니다.
- v: 꼭지점들의 좌표가 들어있는 2차원 행렬입니다.
- t: 삼각형들의 꼭지점 id가 들어있는 2차원 행렬입니다.
- id_org: 삼각형의 id입니다.
- num_frame: 의미없는 변수입니다.
* result_t: 주어진 삼각형을 회전시킨 후의 위치 정보가 들어가 있습니다.
*/
triangle getTriangle(Vertex v[5000], Triangle t[5000], int id_org, int num_frame)
{    
    int id;
	float degree,radian;
    triangle result_t;

	// 원래는 각도를 넘겨주어 transformation을 수행해야하지만
	// set_rotattion함수가 모든 것을 다 하므로 아래 각도 값은 의미가 없습니다.
	degree = 12.0;
	radian = degree*num_frame*PI/180; 

	id = id_org;

	// 아래에서 호출되는 get_rotated_vector 함수의 두번째 인자로
	// id가 id_org인 삼각형(t[id])의 첫번째 Vertex(id값이 t[id].v1-1)의 주소를 넘겨줍니다.
	// 받는 함수 쪽에서는 Vertex 구조체를 float 배열로 생각하게 됩니다. 
	// 참고로 첫번째 인자는 함수내에서 안쓰이고, 세번째 인자에 실행 결과가 들어갑니다.
	get_rotated_vector(radian, (float*) &v[t[id].v1-1], result_t.vert0);
	// 이하 생략
	get_rotated_vector(radian, (float*) &v[t[id].v2-1], result_t.vert1);
	get_rotated_vector(radian, (float*) &v[t[id].v3-1], result_t.vert2);
	
	return result_t;
}