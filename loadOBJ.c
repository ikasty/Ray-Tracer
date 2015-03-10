#include <stdio.h>
#include <string.h>
#include "type.h"
#include "msl_math.h"
#include <math.h>

/* 
�ڵ� �̸��� ���� ������. 
���������� ���� ��ȭ�� ���� ������� ��ǥ�� �ٲ��ִ� �Ϳ� ���� ������ �� �ֽ��ϴ�.
*/

// �ﰢ�� ������Ʈ ��Ű��
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
getTriangle: ȸ���� ������ �� �ﰢ���� ��ǥ�� ��ȯ�ϴ� �Լ��Դϴ�.
- v: ���������� ��ǥ�� ����ִ� 2���� ����Դϴ�.
- t: �ﰢ������ ������ id�� ����ִ� 2���� ����Դϴ�.
- id_org: �ﰢ���� id�Դϴ�.
- num_frame: �ǹ̾��� �����Դϴ�.
* result_t: �־��� �ﰢ���� ȸ����Ų ���� ��ġ ������ �� �ֽ��ϴ�.
*/
triangle getTriangle(Vertex v[5000], Triangle t[5000], int id_org, int num_frame)
{    
    int id;
	float degree,radian;
    triangle result_t;

	// ������ ������ �Ѱ��־� transformation�� �����ؾ�������
	// set_rotattion�Լ��� ��� ���� �� �ϹǷ� �Ʒ� ���� ���� �ǹ̰� �����ϴ�.
	degree = 12.0;
	radian = degree*num_frame*PI/180; 

	id = id_org;

	// �Ʒ����� ȣ��Ǵ� get_rotated_vector �Լ��� �ι�° ���ڷ�
	// id�� id_org�� �ﰢ��(t[id])�� ù��° Vertex(id���� t[id].v1-1)�� �ּҸ� �Ѱ��ݴϴ�.
	// �޴� �Լ� �ʿ����� Vertex ����ü�� float �迭�� �����ϰ� �˴ϴ�. 
	// ����� ù��° ���ڴ� �Լ������� �Ⱦ��̰�, ����° ���ڿ� ���� ����� ���ϴ�.
	get_rotated_vector(radian, (float*) &v[t[id].v1-1], result_t.vert0);
	// ���� ����
	get_rotated_vector(radian, (float*) &v[t[id].v2-1], result_t.vert1);
	get_rotated_vector(radian, (float*) &v[t[id].v3-1], result_t.vert2);
	
	return result_t;
}