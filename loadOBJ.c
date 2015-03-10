#include <stdio.h>
#include <string.h>
#include "type.h"
#include "msl_math.h"
#include <math.h>

//삼각형 로테이트 시키기
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

	// 전역 변수에 값을 넣어 줍니다. 
	//one_minus_cos = 1.0-r_cos;
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
triangle getTriangle(int id_org)
{
	int id, eo;
	triangle f_tri;
	triangle orig_tri;        
	float degree,radian;
	
	int i;
	float move[3];    //각각의 좌표들의 움직인 정도 저장을 위한 배열

	id = id_org>>1;
	eo = id_org%2;
	degree=3.6;
	radian=degree*id*PI/180; //각도 표현을 라디안 표현법으로 고치기

	orig_tri.vert0[0]=10.0;
	orig_tri.vert0[1]=110.0;
	orig_tri.vert0[2]=10.0;
	orig_tri.vert1[0]=100.0;
	orig_tri.vert1[1]=200.0;
	orig_tri.vert1[2]=10.0;
	orig_tri.vert2[0]=200.0;
	orig_tri.vert2[1]=110.0;
	orig_tri.vert2[2]=10.0;


	move[0]=orig_tri.vert1[0];
	move[1]=orig_tri.vert0[1];
	move[2]=orig_tri.vert0[2];
	

	
	if(eo==0){
		f_tri.vert0[0]=get_rotated_vector(radian,id,orig_tri.vert0,move).x;
		f_tri.vert0[1]=get_rotated_vector(radian,id,orig_tri.vert0,move).y;
		f_tri.vert0[2]=get_rotated_vector(radian,id,orig_tri.vert0,move).z;
		f_tri.vert1[0]=get_rotated_vector(radian,id,orig_tri.vert1,move).x;
		f_tri.vert1[1]=get_rotated_vector(radian,id,orig_tri.vert1,move).y;
		f_tri.vert1[2]=get_rotated_vector(radian,id,orig_tri.vert1,move).z;
		f_tri.vert2[0]=get_rotated_vector(radian,id,orig_tri.vert2,move).x;
		f_tri.vert2[1]=get_rotated_vector(radian,id,orig_tri.vert2,move).y;
		f_tri.vert2[2]=get_rotated_vector(radian,id,orig_tri.vert2,move).z;
	}else{
		f_tri.vert0[0]=get_rotated_vector(radian,id,orig_tri.vert2,move).x;
		f_tri.vert0[1]=get_rotated_vector(radian,id,orig_tri.vert2,move).y;
		f_tri.vert0[2]=get_rotated_vector(radian,id,orig_tri.vert2,move).z;
		f_tri.vert1[0]=get_rotated_vector(radian,id,orig_tri.vert1,move).x;
		f_tri.vert1[1]=get_rotated_vector(radian,id,orig_tri.vert1,move).y;
		f_tri.vert1[2]=get_rotated_vector(radian,id,orig_tri.vert1,move).z;
		f_tri.vert2[0]=get_rotated_vector(radian,id,orig_tri.vert0,move).x;
		f_tri.vert2[1]=get_rotated_vector(radian,id,orig_tri.vert0,move).y;
		f_tri.vert2[2]=get_rotated_vector(radian,id,orig_tri.vert0,move).z;
	}

	return f_tri;
}
*/

/* 
getTriangle: 회전을 수행한 후 삼각형의 좌표를 반환하는 함수입니다.
- v: 
- t: 삼각형의 꼭지점 id가 들어있는 행렬입니다.
- id_org
- num_frame: 의미없는 변수입니다.

* result_t
*/
triangle getTriangle(Vertex v[5000], Triangle t[5000], int id_org, int num_frame)
{    
    int id;
	float degree,radian;
	float move[3][3];    //각각의 좌표들의 움직인 정도 저장을 위한 배열
    triangle get_t,result_t;

	// 원래는 각도를 넘겨주어 transformation을 수행해야하지만
	// set_rotattion함수가 모든 것을 다 하므로 아래 각도 값은 의미가 없습니다.
	degree = 12.0;
	radian=degree*num_frame*PI/180; 

	id = id_org;
	
	/*
	get_t.vert0[0]=v[t[id].v1-1].x;
	get_t.vert0[1]=v[t[id].v1-1].y;
	get_t.vert0[2]=v[t[id].v1-1].z;
	get_t.vert1[0]=v[t[id].v2-1].x;
	get_t.vert1[1]=v[t[id].v2-1].y;
	get_t.vert1[2]=v[t[id].v2-1].z;
	get_t.vert2[0]=v[t[id].v3-1].x;
	get_t.vert2[1]=v[t[id].v3-1].y;
	get_t.vert2[2]=v[t[id].v3-1].z;
	*/

	//get_rotated_vector(radian,get_t.vert0, result_t.vert0);
	get_rotated_vector(radian, (float*) &v[t[id].v1-1].x, result_t.vert0);
	//result_t.vert0[1]=get_rotated_vector(radian,get_t.vert0).y;
	//result_t.vert0[2]=get_rotated_vector(radian,get_t.vert0).z;

	get_rotated_vector(radian, (float*) &v[t[id].v2-1].x, result_t.vert1);
	//get_rotated_vector(radian,get_t.vert1, result_t.vert1);
	//result_t.vert1[1]=get_rotated_vector(radian,get_t.vert1).y;
	//result_t.vert1[2]=get_rotated_vector(radian,get_t.vert1).z;

	get_rotated_vector(radian, (float*) &v[t[id].v3-1].x, result_t.vert2);
	//get_rotated_vector(radian,get_t.vert2, result_t.vert2);
	//result_t.vert2[1]=get_rotated_vector(radian,get_t.vert2).y;
	//result_t.vert2[2]=get_rotated_vector(radian,get_t.vert2).z;
	
	return result_t;
}