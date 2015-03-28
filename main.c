#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "main.h"

#include "bitmap_make.h"
#include "intersection.h"
#include "obj_transform.h"
#include "settings.h"
#include "include/debug-msg.h"

// PDEBUG() 디버그 메시지 함수 선언
PDEBUG_INIT();

// 카메라와 광원 선언
DEFINE_CAMERA();
DEFINE_LIGHT();
DEFINE_SCREEN();

// 콘솔 화면에 진행상황을 출력해 줍니다.
static void print_percent(int framenumber, float percent, double spend_time)
{
	int i;

	// 디버그 메시지가 출력되었다면 메시지를 가리지 않도록 함
	DEBUG_ONLY({
		if (__PDEBUG_ENABLED) printf("\n");
		__PDEBUG_ENABLED = 0;
	});

	USE_SCREEN(screen);

	printf("\rframe %02d/%02d: [", framenumber, screen->frame_count);

	for (i = 0; i <= (int)(percent / 5); i++) printf("=");
	for (i = (int)(percent / 5); i < 20; i++) printf(" ");

	printf("] %05.2f%% %.3fs", percent, spend_time);
	if (percent == 100.0f) printf("\n");
	fflush(stdout);
}

int main(int argc, char *argv[])
{
	FILE		*fp = NULL;										// 파일 포인터
	char		obj_file[100];									// 입력 obj 파일 이름 버퍼
	char		default_obj_file[100] = "cube.obj";				// 기본 obj 파일 이름
	char		img_file[100];									// 출력 이미지 파일 이름 버퍼

	int			screen_buffer[X_SCREEN_SIZE * Y_SCREEN_SIZE];	// bmp파일을 위한 색상정보가 들어가는 배열입니다.
	int			index_x, index_y;								// 스크린의 픽셀별로 통과하는 광선의 x, y축 좌표 인덱스
	int			framenumber;									// 현재 이미지 frame 번호

	clock_t		start_clock, end_clock;							// 수행 시간 계산용 clock_t 변수
	double		sum_clock = 0.0;								// 수행 시간 누적 변수

	int			i;												// for문에 쓰는 변수
	char* option;
		

	// 데이터 저장용 구조체
	Data data;

	USE_CAMERA(camera);
	USE_SCREEN(screen);

	// 명령줄 옵션 처리
	// 사용법 : ./RayTracing.exe -c<프레임개수> <obj파일>
	for(i = 1; i < argc; i++) {
		option = argv[i];
		if(strncmp(option, "-c", 2) == 0) {
			screen->frame_count = atoi(option + 2);
			PDEBUG("set frame count = %d\n", screen->frame_count);
			continue;
		} 

		if (fp != NULL) break;
		sprintf(obj_file, "%s", option);
		fp = fopen(option, "r");
		PDEBUG("open %s\n", option);
	}
	if (fp == NULL) {
		fp = fopen(default_obj_file, "r");
		PDEBUG("open %s\n", option);
	}

	// 파일에서 데이터를 불러옵니다
	memset(&data, 0, sizeof(data));
	if (file_read(fp, &data) < 0) return -1;

	for (framenumber = 0; framenumber < screen->frame_count; framenumber++)
	{
		// 현재 frame에서 보여줄 화면 로테이션에 필요한 기본 정보를 집어넣습니다.
		set_rotate(framenumber, screen->frame_count);

		// bmp buffer 배열인 screen_buffer을 초기화해 줍니다.
		memset(screen_buffer, 0, sizeof(screen_buffer));

		// 각 픽셀별로 교차검사를 수행합니다.
		for (index_y = 0; index_y < camera->resy; index_y++)
		{
			float percent;

			// 진행 상태 출력
			percent = ((float)index_y / camera->resy + framenumber) / screen->frame_count * 100.0f;
			print_percent(framenumber, percent, sum_clock);

			// 시작 시간 계산
			start_clock = clock();

			for (index_x = 0; index_x < camera->resx; index_x++)
			{
				Ray f_ray = gen_ray((float)index_x, (float)index_y);
				Hit ist_hit = intersect_search(&data, &f_ray, (float)index_x, (float)index_y);
			
				// bmp파일을 작성에 필요한 색상정보를 입력합니다.
				screen_buffer[X_SCREEN_SIZE * index_y + index_x]
					= Shading(f_ray, getTriangle(data.vert, data.face, ist_hit.triangle_id), ist_hit);
			}

			// 종료 시간 계산
			end_clock = clock();
			sum_clock += (double) (end_clock - start_clock) / CLOCKS_PER_SEC;
		} // index_y
	
		// img_file 변수에 파일 이름을 집어넣어 줍니다.
		sprintf(img_file, "%s.%04d.bmp", obj_file, framenumber);
		
		// 실제 bmp 파일을 만들어 줍니다. screen_buffer 배열에 색상정보가 모두 들어가 있습니다.
		OutputFrameBuffer(X_SCREEN_SIZE, Y_SCREEN_SIZE, screen_buffer, img_file);
	} // index_x

	print_percent(screen->frame_count, 100.0f, sum_clock);
	return 0;
}
