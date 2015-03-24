#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "main.h"

#include "bitmap_make.h"
#include "intersection.h"
#include "obj_transform.h"
#include "debug-msg.h"
#include "settings.h"

Vertex v[5000];
Triangle t[5000];

// PDEBUG() 디버그 메시지 함수 선언
PDEBUG_INIT();

// 콘솔 화면에 진행상황을 출력해 줍니다.
static void print_percent(int framenumber, float percent, double spend_time)
{
	int i;

	// 디버그 메시지가 출력되었다면 메시지를 가리지 않도록 함
	DEBUG_ONLY({
		if (__PDEBUG_ENABLED) printf("\n");
		__PDEBUG_ENABLED = 0;
	});

	printf("\rframe %02d/%02d: [", framenumber, FRAME_COUNT);

	for (i = 0; i <= (int)(percent / 5); i++) printf("=");
	for (i = (int)(percent / 5); i < 20; i++) printf(" ");

	printf("] %05.2f%% %.3fs", percent, spend_time);
	if (percent == 100.0f) printf("\n");
	fflush(stdout);
}

int main(int argc, char *argv[])
{
	FILE *fp;	
	int				screen_buffer[X_SCREEN_SIZE * Y_SCREEN_SIZE];	// bmp파일을 위한 색상정보가 들어가는 배열입니다.
	float			light[3];
	int				index_x, index_y;								// 스크린의 픽셀별로 통과하는 광선의 x, y축 좌표
	int				framenumber;									// 현재 이미지 frame 번호
	char			filename[100];									// 이미지 파일 이름 버퍼
	clock_t			start_clock, end_clock;							// 수행 시간 계산용 clock_t 변수
	double			sum_clock = 0.0;								// 수행 시간 누적 변수

	// 데이터 저장용 구조체
	Data data;

	// 카메라 구조체
	Camera input_cam;

	// 파일에서 데이터를 불러옵니다
	if (argc == 2)
	{
		fp = fopen(argv[1], "r");
		PDEBUG("open %s\n", argv[1]);
	}
	else
	{
		fp = fopen("cube.obj", "r");
		PDEBUG("open cube.org\n");
	}


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

	print_percent(0, 0.0f, 0.0f);

	for (framenumber = 0; framenumber < FRAME_COUNT; framenumber++)
	{
		// 현재 frame에서 보여줄 화면 로테이션에 필요한 기본 정보를 집어넣습니다.
		set_rotate(framenumber);

		// bmp buffer 배열인 screen_buffer을 초기화해 줍니다.
		memset(screen_buffer, 0, sizeof(screen_buffer));

		// 각 픽셀별로 교차검사를 수행합니다.
		for (index_y = 0; index_y < input_cam.resy; index_y++)
		{
			float percent;

			// 시작 시간 계산
			start_clock = clock();

			for (index_x = 0; index_x < input_cam.resx; index_x++)
			{
				Ray f_ray = gen_ray(input_cam, (float)index_x, (float)index_y);
				Hit ist_hit = intersect_search(&data, &f_ray, (float)index_x, (float)index_y);
			
				// bmp파일을 작성에 필요한 색상정보를 입력합니다.
				screen_buffer[X_SCREEN_SIZE * index_y + index_x]
					= Shading(f_ray, getTriangle(data.vert, data.face, ist_hit.triangle_id), ist_hit);
			}

			// 종료 시간 계산
			end_clock = clock();
			sum_clock += (double) (end_clock - start_clock) / CLOCKS_PER_SEC;

			percent = ((float)index_y / input_cam.resy + framenumber) / FRAME_COUNT * 100.0f;
			print_percent(framenumber, percent, sum_clock);

		} // index_y
	
		// filename 변수에 파일 이름을 집어넣어 줍니다.
		sprintf(filename, "out_%03d.bmp", framenumber);
		
		// 실제 bmp 파일을 만들어 줍니다. screen_buffer 배열에 색상정보가 모두 들어가 있습니다.
		OutputFrameBuffer(X_SCREEN_SIZE, Y_SCREEN_SIZE, screen_buffer, filename);
	} // index_x

	print_percent(FRAME_COUNT, 100.0f, sum_clock);
	return 0;
}
