//#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include "main.h"

// include search and render algorithm
#include "algorithms.h"

// shading algorithm
#include "shading/shading.h"

//#include "bitmap_make.h"
#include "obj_transform.h"
#include "timecheck.h"
#include "settings.h"
#include "images/image_read.h"

#include "include/getopt.h"
#include "include/debug-msg.h"

// PDEBUG() 디버그 메시지 함수 선언
PDEBUG_INIT();

// 콘솔 화면에 진행상황을 출력해 줍니다.
static void print_percent(int frame_number, float percent)
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

	printf("] %06.3f%%, total %.3fs", percent, get_total_clock());
	if (percent == 100.0f)
	{
		printf("\n");
		printf(	" acc. object build\t"	"%.3fs\n"
				" acc. object search\t"	"%.3fs\n"
				" intersect check\t"	"%.3fs\n"
				" render\t\t\t"			"%.3fs\n"
				" total\t\t\t"			"%.3fs\n",
			get_build_clock(),
			get_search_clock(),
			intersect_clock,
			get_render_clock(),
			get_total_clock());
	}
	fflush(stdout);
}

static void do_algorithm(Data *data, char *input_file)
{
	char		output_file[100];			// 출력 이미지 파일 이름 버퍼

	Image*	screen_buffer = NULL;				// bmp파일을 위한 색상정보가 들어가는 배열입니다.
	int			index_x, index_y;			// 스크린의 픽셀별로 통과하는 광선의 x, y축 좌표 인덱스
	int			frame_number;				// 현재 이미지 frame 번호

	USE_SCREEN(screen);
	USE_TIMECHECK();

	//screen_buffer = (int*)malloc(sizeof(int) * screen->xsize * screen->ysize);
 screen_buffer = image_init(screen->xsize, screen->ysize); 

	for (frame_number = 0; frame_number < screen->frame_count; frame_number++)
	{
	//// -- pre-step phase --
	PDEBUG("main.c pre-step phase\n");

		// 우선 해당 frame_number에 맞게 object를 회전합니다.
		if (frame_number)
		{
			int i, j;
			for (i = 0; i < data->prim_count; i++)
			{
				for (j = 0; j < 3; j++)
				{
					get_rotated_vector(data->primitives[i].vert[j]);
					get_rotated_vector(data->primitives[i].norm[j]);
				}
			}
		}

		// 이미지 버퍼를 초기화해 줍니다.
		//memset(screen_buffer, 0, sizeof(int) * screen->xsize * screen->ysize);
		image_reset(screen_buffer);

	//// -- execute phase --
	PDEBUG("main.c execute phase\n");

		// 만약 가속구조체를 사용한다면 빌드함
		if (accel_build)
		{
			// 기존 구조체 해제
			if (clear_accel) (*clear_accel)(data);

			TIMECHECK_START();
			(*accel_build)(data);
			TIMECHECK_END(build_clock);

			PDEBUG("accel_build finished\n");
		}

		// 각 픽셀별로 교차검사를 수행합니다.
		for (index_y = 0; index_y < screen->ysize; index_y++)
		{
			float percent;

			// 진행 상태 출력
			percent = ((float)index_y / screen->ysize + frame_number) / screen->frame_count * 100.0f;
			print_percent(frame_number, percent);

			for (index_x = 0; index_x < screen->xsize; index_x++)
			{
				Ray f_ray = gen_ray((float)index_x, (float)index_y);
				Hit ist_hit;

				// 현재 광선에서 교차검사를 수행함
				TIMECHECK_START();
				ist_hit = (*intersect_search)(data, &f_ray);
				TIMECHECK_END(search_clock);

				// bmp파일을 작성에 필요한 색상정보를 입력합니다.
				if (ist_hit.t > 0)
				{
					//unsigned int *pixel = &screen_buffer[screen->xsize * index_y + index_x];
     RGBA *pixel = &(screen_buffer->pixels[index_y][index_x]);

					// 교차된 Primitive가 있다면 렌더링함
					TIMECHECK_START();
					*pixel = shading(f_ray, data->primitives[ist_hit.prim_id], ist_hit, data);
					TIMECHECK_END(render_clock);
				}
			}

		} // index_y
	
	//// -- post-step phase --
	PDEBUG("main.c post-step phase\n");

		// output_file 변수에 파일 이름을 집어넣어 줍니다.
		sprintf(output_file, "%s.%04d.bmp", input_file, frame_number + 1);
		
		// 실제 bmp 파일을 만들어 줍니다.
		//OutputFrameBuffer(screen->xsize, screen->ysize, screen_buffer, output_file);
		image_write(screen_buffer, output_file, IMAGE_NO_FLAGS);
	} // index_x

	print_percent(frame_number, 100.0f);

	free(screen_buffer);
}

int main(int argc, char *argv[])
{
	FILE	*fp = NULL;					// 파일 포인터
	char	input_file[100] = {0,};		// 입력 obj 파일 이름 버퍼
	Data	data;						// 데이터 저장용 구조체
	float	scale = 1.0f;				// 화면 scale용 변수

	USE_SCREEN(screen);

	// -- 명령줄 옵션 처리
	char c;
	while ((c = getopt(argc, argv, "hc:a:f:s:S:")) != -1)
	{
long_option:
		switch (c)
		{
		case '-':
			if (strncmp(optarg, "count", 5) == 0)		c = 'c', optarg += 5;
			else if (strncmp(optarg, "help", 4) == 0)	c = 'h', optarg += 4;
			else if (strncmp(optarg, "file", 4) == 0)	c = 'f', optarg += 4;

			if (*optarg == '=') optarg++;
			goto long_option;

		case 'c':
			screen->frame_count = atoi(optarg);
			printf("set frame count = %d\n", screen->frame_count);
			break;

		case 'a':
			init_search_algo(optarg);
			break;

		case 's':
			init_shading_algo(optarg);
			break;

		case 'S':
			scale = (float)atof(optarg);
			printf("object scale to %f\n", scale);
			break;

		case 'f':
			if (input_file[0] != '\0') break;
			printf("set filename %s\n", optarg);
			sprintf(input_file, "%s", optarg);
			break;

		// program will terminated!
		case '?':
		default:
			printf("unknown command %s\n!", argv[optind - 1]);
		case 'h':
			printf(
				"Usage: ./RayTracing.exe [options] [filename]\n"
				"Options:\n"
				"  -c COUNT, --count=COUNT\t\t"			"Set frame count.\n"
				"  -a (naive|nlog2n|nlongn)\t\t"		"Set search algorithm.\n"
				"  -s (naive|advanced)\t\t\t"			"Set shading algorithm.\n"
				"  -S SCALE\t\t\t\t"					"Set object scale factor\n"
				"  -f FILENAME, --file=FILENAME\t\t"	"Set obj filename.\n"
				"  -h, --help\t\t\t\t"					"Print this message and exit.\n");

			return (c == 'h') ? 0 : -1;
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

	// 기본 알고리즘 선택
	init_search_algo("");
	init_shading_algo("");
	// -- 명령줄 옵션 처리 끝
	
	// 파일 열기
	fp = fopen(input_file, "r");
	PDEBUG("open %s\n", input_file);

	// 파일에서 데이터를 불러옵니다
	memset(&data, 0, sizeof(data));
	if (file_read(fp, &data, scale) < 0) return -1;

	// 이미지 읽기 테스트
	if(image_read(&(data.texture), DEFAULT_TEXTURE_FILE, IMAGE_NO_FLAGS))
		PDEBUG("cannot open image file : %s", DEFAULT_TEXTURE_FILE);
	// 이미지 쓰기 테스트
	if(image_write(&(data.texture), "texture_save_test.bmp", IMAGE_NO_FLAGS))
		PDEBUG("cannot save image file : %s", "texture_save_test.bmp");

	// 화면 로테이션에 필요한 기본 정보를 집어넣습니다.
	set_rotate(screen->frame_count);

	// 알고리즘을 수행합니다.
	do_algorithm(&data, input_file);

	return 0;
}
