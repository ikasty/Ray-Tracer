#include <stdio.h>
#include <string.h>
#include "type.h"
#include "main.h"

#define x_screen 320
#define y_screen 240

float distance=100;

Vertex v[5000];
	Triangle t[5000];

void set_rotate(int num_frame);

int main()
{	
	
	msl_ray f_ray;
	hit ist_hit;
	camera input_cam;

	

	//int screen[y_screen][x_screen];
	int screen_1_dim[x_screen*y_screen];
	float light[3];
	unsigned int color;
	int triangle_id;
	int maxNumOfTri=100;
	int index_x,index_y;          //��ũ���� �ȼ����� ����ϴ� ���� �˻��ϱ� ���� ����
	int framenumber;
	char filename[100];
	int vertexCount = 0;
    int triangleCount = 0;
    char line[100];
	int num_of_frame=0;
   	FILE *fp = fopen("cube.obj","r");
	

	//����
	light[0]=100.0;
	light[1]=300.0;
	light[2]=10.0;
	
	//������ ������ ������
	input_cam.orig[0]=-100.0;
	input_cam.orig[1]=-50.0;
	input_cam.orig[2]=-1000.0;
	
	input_cam.distance=distance;
	input_cam.resx=x_screen;
	input_cam.resy=y_screen;

	
   
	
	
	if (fp != NULL)
	{
		while (fgets(line, 99, fp))
			{
				if (line[0] == 'v')
				{
					sscanf(line, "%*c %f %f %f", &v[vertexCount].x, &v[vertexCount].y, &v[vertexCount].z);                  
					vertexCount++;
				} 
				else if (line[0] == 'f')
				{
					sscanf(line, "%*c %d %d %d",  &t[triangleCount].v1, &t[triangleCount].v2, &t[triangleCount].v3);
					triangleCount++;
				}   
		}   
	}
	
	 
	for(framenumber=0;framenumber<30;framenumber++)
	{

		
	
		set_rotate(num_of_frame);
		printf("\r\n");
	
		for(index_y=0;index_y<input_cam.resy;index_y++)
		{
			for(index_x=0;index_x<input_cam.resx;index_x++)
			{
				float min_t= 1000000;	
			
				f_ray=gen_ray(input_cam,index_x+input_cam.orig[0],index_y+input_cam.orig[1]);

				for(triangle_id=0;triangle_id<triangleCount*2;triangle_id++)
				{
					
					ist_hit=intersect_triangle(f_ray,getTriangle(v,t,triangle_id,num_of_frame));
					
					if(ist_hit.t>0 && min_t>ist_hit.t)
					{
					min_t=ist_hit.t;
					color= Shading(f_ray,getTriangle(v,t,triangle_id,num_of_frame),ist_hit);	
					screen_1_dim[x_screen*index_y+index_x]=	color;
					}
					/*
					if(shadow_test(f_ray,light,getTriangle("cube.obj",triangle_id))==1)
					{
						color=0xffff0000;
						screen_1_dim[x_screen*index_y+index_x]=	color;
					}
					*/
					
				}
				

				
				

			}
			printf("frame %03d: %5.2f %%\r",framenumber, index_y*100.0f/input_cam.resy);
		}

		printf("\n");
	
		sprintf(filename,"out_%03d.bmp",framenumber);
		
		OutputFrameBuffer(x_screen, y_screen, screen_1_dim, filename);
		printf("out_%03d.bmp",framenumber);
		num_of_frame++;
	}	
	
	return 0;
}