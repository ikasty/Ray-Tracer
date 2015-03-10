#include <stdio.h>
#include <string.h>
#include "type.h"
#include "msl_math.h"
#include <math.h>




//삼각형 로테이트 시키기

float r_cos;
float r_sin;

float one_minus_cos;

float result[3],get_vect_by_four[3],cal__[3];
float transformation[3][3];
	
coordinate r_vec;



void get_rotated_vector(float theta,float getvector[3], float Outvector[3])
{

	int i,j;
	

	//transformation행렬과 곱하기 위해 3x1행렬을 4x1행렬로
	get_vect_by_four[0]=getvector[0];
	get_vect_by_four[1]=getvector[1];
	get_vect_by_four[2]=getvector[2];


	for(i=0;i<3;i++)//행렬의 곱셈 연산
	{
		for(j=0;j<3;j++)
		{
			cal__[j]= get_vect_by_four[j]*transformation[j][i];                             
		}
		Outvector[i]=cal__[0]+cal__[1]+cal__[2];

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
void set_rotate(int num_frame){

	float degree,radian;

	degree=12.0;
	radian=degree*num_frame*PI/180; //각도 표현을 라디안 표현법으로 고치기
	
	r_cos=cos(radian);
	r_sin=sin(radian);

	one_minus_cos=1.0-r_cos;

	result[3],get_vect_by_four[3],cal__[3];
	transformation[3][3];
	
	memset(transformation,0,sizeof(transformation));
	//rotate
	transformation[0][0]=1;
	transformation[1][1]=r_cos;
	transformation[1][2]=r_sin;
	transformation[2][1]=-r_sin;
	transformation[2][2]=r_cos;


}



triangle getTriangle(Vertex v[5000],Triangle t[5000],int id_org,int num_frame)
{    
    
    int id, eo;
	float degree,radian;
	float move[3][3];    //각각의 좌표들의 움직인 정도 저장을 위한 배열
    triangle get_t,result_t;

	degree=12.0;
	radian=degree*num_frame*PI/180; //각도 표현을 라디안 표현법으로 고치기

	id = id_org;
	eo = 0;
	

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