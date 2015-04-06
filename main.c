﻿#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "main.h"

// naive
#include "intersection.h"

// kdtree(nlog^2n)
#include "kdtree/kdtree_build.h"
#include "kdtree/kdtree_intersection.h"

#include "bitmap_make.h"
#include "obj_transform.h"
#include "settings.h"
#include "include/debug-msg.h"

// PDEBUG() 디버그 메시지 함수 선언
PDEBUG_INIT();

// 카메라와 광원 선언
DEFINE_CAMERA();
DEFINE_LIGHT();
DEFINE_SCREEN();

// accelaration 구조체 설정 함수
#if defined(ACCEL_OPTION) || defined(ACCEL_USE_ASK)
	void (*accel_build)(Data *data) = &kdtree_accel_build;
	Hit (*intersect_search)(Data *data, Ray *ray) = &kdtree_intersect_search;
#else
	void (*accel_build)(Data *data) = NULL;
	Hit (*intersect_search)(Data *data, Ray *ray) = &naive_intersect_search;
#endif

// 콘솔 화면에 진행상황을 출력해 줍니다.
static void print_percent(int frame_number, float percent, double build_clock, double search_clock)
{
	int i;
	USE_SCREEN(screen);

	// 디버그 메시지가 출력되었다면 메시지를 가리지 않도록 함
	DEBUG_ONLY({
		if (__PDEBUG_ENABLED) printf("\n");
		__PDEBUG_ENABLED = 0;
	});

	
	printf("\rframe %02d/%02d: [", frame_number, screen->frame_count);

	for (i = 0; i <= (int)(percent / 5); i++) printf("=");
	for (i = (int)(percent / 5); i < 20; i++) printf(" ");

	printf("] %05.2f%% %.3fs build, %.3fs search", percent, build_clock, search_clock);
	if (percent == 100.0f) printf("\n");
	fflush(stdout);
}

static void do_algorithm(Data *data, char *input_file)
{
	char		output_file[100];								// 출력 이미지 파일 이름 버퍼

	int			screen_buffer[X_SCREEN_SIZE * Y_SCREEN_SIZE];	// bmp파일을 위한 색상정보가 들어가는 배열입니다.
	int			index_x, index_y;								// 스크린의 픽셀별로 통과하는 광선의 x, y축 좌표 인덱스
	int			frame_number;									// 현재 이미지 frame 번호

	clock_t		start_clock, end_clock;							// 수행 시간 계산용 clock_t 변수
	double		build_clock = 0.0;								// 가속체 구성 시간 누적 변수
	double		search_clock = 0.0;								// 탐색 시간 누적 변수

	USE_SCREEN(screen);
	USE_CAMERA(camera);

	for (frame_number = 0; frame_number < screen->frame_count; frame_number++)
	{
	//// -- pre-step phase --

		// 우선 해당 frame_number에 맞게 object를 회전합니다.
		if (frame_number)
		{
			int i;
			for (i = 0; i < data->prim_count; i++)
			{
				get_rotated_vector(data->primitives[i].vert0);
				get_rotated_vector(data->primitives[i].vert1);
				get_rotated_vector(data->primitives[i].vert2);
			}
		}

		// bmp buffer 배열인 screen_buffer을 초기화해 줍니다.
		memset(screen_buffer, 0, sizeof(screen_buffer));

	//// -- execute phase --

		// 만약 가속구조체를 사용한다면 빌드함
		if (accel_build)
		{
			start_clock = clock();
			(*accel_build)(data);
			end_clock = clock();

			build_clock += (double) (end_clock - start_clock) / CLOCKS_PER_SEC;
		}

		// 각 픽셀별로 교차검사를 수행합니다.
		for (index_y = 0; index_y < camera->resy; index_y++)
		{
			float percent;

			// 진행 상태 출력
			percent = ((float)index_y / camera->resy + frame_number) / screen->frame_count * 100.0f;
			print_percent(frame_number, percent, build_clock, search_clock);

			// 시작 시간 계산
			start_clock = clock();

			for (index_x = 0; index_x < camera->resx; index_x++)
			{
				Ray f_ray = gen_ray((float)index_x, (float)index_y);
				Hit ist_hit = (*intersect_search)(data, &f_ray);

				// bmp파일을 작성에 필요한 색상정보를 입력합니다.
				if (ist_hit.t > 0)
				{
					screen_buffer[X_SCREEN_SIZE * index_y + index_x]
						= Shading(f_ray, data->primitives[ist_hit.prim_id], ist_hit);
				}
			}

			// 종료 시간 계산
			end_clock = clock();
			search_clock += (double) (end_clock - start_clock) / CLOCKS_PER_SEC;
		} // index_y
	
	//// -- post-step phase --

		// output_file 변수에 파일 이름을 집어넣어 줍니다.
		sprintf(output_file, "%s.%04d.bmp", input_file, frame_number);
		
		// 실제 bmp 파일을 만들어 줍니다. screen_buffer 배열에 색상정보가 모두 들어가 있습니다.
		OutputFrameBuffer(X_SCREEN_SIZE, Y_SCREEN_SIZE, screen_buffer, output_file);
	} // index_x

	print_percent(frame_number, 100.0f, build_clock, search_clock);
}

int main(int argc, char *argv[])
{
	FILE	*fp = NULL;					// 파일 포인터
	char	input_file[100] = {0,};		// 입력 obj 파일 이름 버퍼
	Data	data;						// 데이터 저장용 구조체

	USE_SCREEN(screen);

	// -- 명령줄 옵션 처리
	char c;
	while ((c = getopt(argc, argv, "hc:a:f:")) != -1)
	{
long_option:
		switch (c)
		{
		case '-':
			if (strncmp(optarg, "count", 5) == 0)		c = 'c', optarg += 5;
			else if (strncmp(optarg, "help", 4) == 0)	c = 'h', optarg += 4;
			else if (strncmp(optarg, "file", 4) == 0)	c = 'f', optarg += 4;

			if (*(optarg++) != '=') c = '?';
			goto long_option;
		case 'h':
			printf(
				"usage: ./RayTracing.exe -cafh [filename]\n"
				"\t-c[count], --count: set frame count\n"
				"\t-a(naive|kdtree): algorithm set\n"
				"\t-f[filename], --file: set obj filename\n"
				"\t-h, --help: print this usage\n");
			return 0;
		case 'c':
			screen->frame_count = atoi(optarg);
			PDEBUG("set frame count = %d\n", screen->frame_count);
			break;
		case 'a':
		#ifdef ACCEL_USE_ASK
			if (strncmp(optarg, "naive", 5) == 0)
			{
				PDEBUG("use naive algorithm\n");
				accel_build = NULL;
				intersect_search = &naive_intersect_search;
			}
			else if (strncmp(optarg, "kdtree", 6) == 0)
			{
				PDEBUG("use kdtree nlog^2n algorithm\n");
			}
		#endif
			break;
		case 'f':
			if (input_file[0] != '\0') break;
			PDEBUG("set filename %s\n", optarg);
			sprintf(input_file, "%s", optarg);
			break;

		case '?':
		default:
			PDEBUG("unknown command %s\n!", argv[optind - 1]);
			return -1;
		}
	}
	
	// 파일명 예외 처리 수행
	if (input_file[0] == '\0')
	{
		if (optind < argc)
			sprintf(input_file, "%s", argv[optind]);
		else
			sprintf(input_file, "%s", DEFAULT_OBJ_FILE);
	}
	// -- 명령줄 옵션 처리 끝
	
	// 파일 열기
	fp = fopen(input_file, "r");
	PDEBUG("open %s\n", input_file);

	// 파일에서 데이터를 불러옵니다
	memset(&data, 0, sizeof(data));
	if (file_read(fp, &data) < 0) return -1;

	// 화면 로테이션에 필요한 기본 정보를 집어넣습니다.
	set_rotate(screen->frame_count);

	// 알고리즘을 수행합니다.
	do_algorithm(&data, input_file);

	return 0;
}
