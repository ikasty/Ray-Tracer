#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include "type.h"
#include "main.h"
#include "file_read.h"

#define x_screen 320
#define y_screen 240

float distance = 100;

Vertex v[5000];
Triangle t[5000];

void set_rotate(int num_frame);

int main()
{		
	msl_ray f_ray;
	hit ist_hit;
	camera input_cam;

	//int screen[y_screen][x_screen];

	// bmp파일을 위한 색상정보가 들어가는 배열입니다.
	int screen_buffer[x_screen * y_screen];
	float light[3];
	unsigned int color;
	int triangle_id;
	int maxNumOfTri = 100;
	int index_x, index_y;          //스크린의 픽셀별로 통과하는 레이 검색하기 위한 변수
	int framenumber;
	char filename[100];
	int vertexCount = 0;
    int triangleCount = 0;
	int num_of_frame = 0;

	// 데이터 저장용 구조체
	struct Data data;

	FILE *fp = fopen("cube.obj", "r");;
	
	//광원
	light[0] = 100.0;
	light[1] = 300.0;
	light[2] = 10.0;
	
	//추적할 레이의 시작점
	input_cam.orig[0] = -100.0;
	input_cam.orig[1] = -50.0;
	input_cam.orig[2] = -1000.0;
	
	input_cam.distance = distance;
	input_cam.resx = x_screen;
	input_cam.resy = y_screen;

	// 파일에서 데이터를 불러옵니다
	memset(&data, 0, sizeof(data));
	if ( file_read(fp, &data) < 0 ) return -1;

	// Data 구조체를 그냥 사용하는게 좋겠지만, 호환성을 위해 변경함
	memcpy(v, data.vert, sizeof(Vertex) * data.vert_count);
	memcpy(t, data.face, sizeof(Triangle) * data.face_count);
	vertexCount = data.vert_count;
	triangleCount = data.face_count;
	
	for (framenumber = 0; framenumber < 30; framenumber++)
	{
		// 로테이션 작업에 필요한 기본 정보를 집어넣습니다.
		set_rotate(num_of_frame);
		// bmp buffer 배열인 screen_buffer을 초기화해 줍니다.
		memset(screen_buffer, 0, sizeof(screen_buffer));
	
		for (index_y = 0; index_y < input_cam.resy; index_y++)
		{
			for (index_x = 0; index_x < input_cam.resx; index_x++)
			{
				float min_t = 1000000;
			
				f_ray = gen_ray(input_cam, (float)index_x + input_cam.orig[0], (float)index_y + input_cam.orig[1]);

				for (triangle_id = 0; triangle_id < triangleCount * 2; triangle_id++)
				{
					ist_hit = intersect_triangle( f_ray, getTriangle(v, t, triangle_id, num_of_frame) );
					
					if (ist_hit.t > 0 && min_t > ist_hit.t)
					{
						min_t=ist_hit.t;
						color = Shading(f_ray, getTriangle(v, t, triangle_id, num_of_frame), ist_hit);
						// bmp파일을 작성에 필요한 색상정보를 입력합니다.
						screen_buffer[x_screen * index_y + index_x] = color;
					}
					/*
					if(shadow_test(f_ray,light,getTriangle("cube.obj",triangle_id))==1)
					{
						color=0xffff0000;
						screen_buffer[x_screen*index_y+index_x]= color;
					}
					*/
				}
			}
			// 콘솔 화면에 진행상황을 퍼센트 형식으로 출력해 줍니다. 
			// \r을 쓰면 첫행으로 돌아가 덮어쓰게됩니다.
			printf("frame %03d: %5.2f %%\r", framenumber, index_y * 100.0f / input_cam.resy);
		}
		printf("frame %03d: %5.2f %%\n", framenumber, index_y * 100.0f / input_cam.resy);
	
		// filename 변수에 파일 이름을 집어넣어 줍니다.
		sprintf(filename, "out_%03d.bmp", framenumber);
		
		// 실제 bmp 파일을 만들어 줍니다. screen_buffer 배열에 색상정보가 모두 들어가 있습니다.
		OutputFrameBuffer(x_screen, y_screen, screen_buffer, filename);

		// 새로 생긴 파일 이름을 화면에 출력해 줍니다.
		printf("out_%03d.bmp\n", framenumber);
		num_of_frame++;
	}
	return 0;
}