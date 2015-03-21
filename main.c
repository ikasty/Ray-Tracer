#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include "main.h"

#include "bitmap_make.h"
#include "intersection.h"
#include "obj_transform.h"
#include "debug-msg.h"
#include "settings.h"

int main()
{		
	int				screen_buffer[X_SCREEN_SIZE * Y_SCREEN_SIZE];	// bmp파일을 위한 색상정보가 들어가는 배열입니다.
	float			light[3];
	int				index_x, index_y;								// 스크린의 픽셀별로 통과하는 광선의 x, y축 좌표
	int				framenumber;									// 현재 이미지 frame 번호
	char			filename[100];									// 이미지 파일 이름 버퍼

	// 데이터 저장용 구조체
	Data data;

	// 카메라 구조체
	Camera input_cam;

	// 파일에서 데이터를 불러옵니다
	FILE *fp = fopen("cube.obj", "r");;
	memset(&data, 0, sizeof(data));
	if (file_read(fp, &data) < 0) return -1;

	// 광원 설정
	light[0] = LIGHT_POS_X;
	light[1] = LIGHT_POS_Y;
	light[2] = LIGHT_POS_Z;

	//추적할 레이의 시작점
	input_cam.orig[0] = CAMERA_POS_X;
	input_cam.orig[1] = CAMERA_POS_Y;
	input_cam.orig[2] = CAMERA_POS_Z;

	input_cam.distance = CAMERA_DISTANCE;
	input_cam.resx = X_SCREEN_SIZE;
	input_cam.resy = Y_SCREEN_SIZE;

	for (framenumber = 0; framenumber < FRAME_COUNT; framenumber++)
	{
		// 현재 frame에서 보여줄 화면 로테이션에 필요한 기본 정보를 집어넣습니다.
		set_rotate(framenumber);

		// bmp buffer 배열인 screen_buffer을 초기화해 줍니다.
		memset(screen_buffer, 0, sizeof(screen_buffer));

		// 각 픽셀별로 교차검사를 수행합니다.
		for (index_y = 0; index_y < input_cam.resy; index_y++)
		{
			int i;
			float percent;

			for (index_x = 0; index_x < input_cam.resx; index_x++)
			{
/*
				Ray f_ray;
				Hit ist_hit;
				int triangle_id;
				float min_t = 1000000;

				// 현재 좌표에서의 광선 구조체를 구함
				f_ray = gen_ray(input_cam, (float)index_x, (float)index_y);

				// 각각의 triangle과 f_ray 광선의 교차 검사를 수행함
				// TODO: 교차 검사 수행을 줄일 알고리즘을 도입할 것
				for (triangle_id = 0; triangle_id < triangleCount; triangle_id++)
				{
					ist_hit = intersect_triangle(f_ray, getTriangle(v, t, triangle_id));

					if (ist_hit.t > 0 && min_t > ist_hit.t)
					{
						DWORD color;

						min_t = ist_hit.t;
						color = Shading(f_ray, getTriangle(v, t, triangle_id), ist_hit);
						// bmp파일을 작성에 필요한 색상정보를 입력합니다.
						screen_buffer[X_SCREEN_SIZE * index_y + index_x] = color;
					}
				}
			} // index_x
*/
				Ray f_ray = gen_ray(input_cam, (float)index_x, (float)index_y);
				Hit ist_hit = intersect_search(&data, &f_ray, (float)index_x, (float)index_y);
			
				// bmp파일을 작성에 필요한 색상정보를 입력합니다.
				screen_buffer[X_SCREEN_SIZE * index_y + index_x]
					= Shading(f_ray, getTriangle(data.vert, data.face, ist_hit.triangle_id), ist_hit);

				DEBUG_ONLY({
					static int debug_count = 0;
					if (f_ray.max_t < MAX_RENDER_DISTANCE && debug_count < 20)
					{
						debug_count++;
						printf("f_ray found triangle #%d(%f, %f, %f) at (%d, %d), color 0x%08x\n",
							ist_hit.triangle_id, ist_hit.t, ist_hit.u, ist_hit.v,
							index_x, index_y,
							screen_buffer[X_SCREEN_SIZE * index_y + index_x]);
					}
				});
			}

			// 콘솔 화면에 진행상황을 퍼센트 형식으로 출력해 줍니다.
			percent = ((float)index_y / input_cam.resy + framenumber) / FRAME_COUNT * 100.0f;

			printf("frame %02d/%2d: [", framenumber, FRAME_COUNT);

			for (i = 0; i < (percent / 5); i++) printf("=");
			for (i = (int)(percent / 5); i < 20; i++) printf(" ");

			printf("] %05.2f%%\r", percent);

		} // index_y
		//printf("frame %03d: %5.2f %%\n", framenumber, index_y * 100.0f / input_cam.resy);
	
		// filename 변수에 파일 이름을 집어넣어 줍니다.
		sprintf(filename, "out_%03d.bmp", framenumber);
		
		// 실제 bmp 파일을 만들어 줍니다. screen_buffer 배열에 색상정보가 모두 들어가 있습니다.
		OutputFrameBuffer(X_SCREEN_SIZE, Y_SCREEN_SIZE, screen_buffer, filename);
	}

	printf("frame %02d/%2d: [====================] %05.2f%%\n", FRAME_COUNT, FRAME_COUNT, 100.0f);
	return 0;
}
