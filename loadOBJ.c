#include <stdio.h>
#include <string.h>
#include "type.h"
#include "msl_math.h"
#include <math.h>

//�ﰢ�� ������Ʈ ��Ű��
// �Ե� ����ð��� ȸ�� transformation�Ҷ� ���� ��� �����?
// �̰� �װ̴ϴ�. ���ƾƾ�!
float transformation[3][3];

/* 
set_rotate: ���� frame ��ȣ�� �������� �����̼ǿ� �ʿ��� �⺻ ������ ä��� �Լ��Դϴ�.
- num_frame: ���° frame������ ��Ÿ���� ���Դϴ�.
*/
void set_rotate(int num_frame){
	float degree,radian;
	float r_cos, r_sin;

	// frame�� 30���̹Ƿ� �� frame�� 12���� ���ư��� ������� �ֽ��ϴ�.
	degree = 12.0;
	// ���� ǥ���� ���� ǥ�������� ��ġ��
	radian=degree*num_frame*PI/180; 
	
	r_cos=cos(radian);
	r_sin=sin(radian);
	
	// transformation �迭�� ��� ���� 0���� �ʱ�ȭ �մϴ�.
	memset(transformation, 0 ,sizeof(transformation));
	// transformation ���࿡ �ʿ��� ��Ŀ� ���� ����ֽ��ϴ�.
	transformation[0][0]=1;
	transformation[1][1]=r_cos;
	transformation[1][2]=-r_sin;
	transformation[2][1]=r_sin;
	transformation[2][2]=r_cos;

	// ���� ������ ���� �־� �ݴϴ�. 
	//one_minus_cos = 1.0-r_cos;
}

/* 
get_rotated_vector: vector�� ȸ����Ű�� �Լ��Դϴ�. 
- theta: ���� ������ ����� �ɰ� ������ ������ �ƹ� ��� ����
- original_vector: ȸ���� ��Ű�� ���� vector�Դϴ�.
- rotated_vector: ȸ�� ����� �� vector�Դϴ�.
*/
void get_rotated_vector(float theta, float *original_vector, float *rotated_vector)
{
	int i,j;

	// transformation�� �����Ͽ� ���� ��ǥ���� ȸ�� �� ��ǥ���� ã���ϴ�.
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
	float move[3];    //������ ��ǥ���� ������ ���� ������ ���� �迭

	id = id_org>>1;
	eo = id_org%2;
	degree=3.6;
	radian=degree*id*PI/180; //���� ǥ���� ���� ǥ�������� ��ġ��

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
getTriangle: ȸ���� ������ �� �ﰢ���� ��ǥ�� ��ȯ�ϴ� �Լ��Դϴ�.
- v: 
- t: �ﰢ���� ������ id�� ����ִ� ����Դϴ�.
- id_org
- num_frame: �ǹ̾��� �����Դϴ�.

* result_t
*/
triangle getTriangle(Vertex v[5000], Triangle t[5000], int id_org, int num_frame)
{    
    int id;
	float degree,radian;
	float move[3][3];    //������ ��ǥ���� ������ ���� ������ ���� �迭
    triangle get_t,result_t;

	// ������ ������ �Ѱ��־� transformation�� �����ؾ�������
	// set_rotattion�Լ��� ��� ���� �� �ϹǷ� �Ʒ� ���� ���� �ǹ̰� �����ϴ�.
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