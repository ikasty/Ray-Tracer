
/*
Copyright (c) 2015, Minwoo Lee(hellomw@msl.yonsei.ac.kr)
					Daeyoun Kang(mail.ikasty@gmail.com),
                    HyungKwan Park(rpdladps@gmail.com),
                    Ingyu Kim(goracom0@gmail.com),
                    Jungmin Kim(kukakhan@gmail.com)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies, 
either expressed or implied, of the FreeBSD Project.
*/

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "main.h"

// include search and render algorithm
#include "algorithms.h"

// do algorithm
#include "do_algorithm.h"

// glut system
#include "GL/glut.h"
#include "interface/opengl.h"

#include "obj_transform.h"
#include "timecheck.h"
#include "settings.h"
#include "images/image_read.h"

#include "include/getopt.h"
#include "include/debug-msg.h"

// PDEBUG() 디버그 메시지 함수 선언
PDEBUG_INIT();

// opengl 사용 여부 flag
static int use_opengl = true;

// 콘솔 화면에 진행상황을 출력해 줍니다.
static void print_percent(float percent)
{
	int i;
	USE_SCREEN(screen);

	// 디버그 메시지가 출력되었다면 메시지를 가리지 않도록 함
	DEBUG_ONLY({
		if (__PDEBUG_ENABLED) printf("\n");
		__PDEBUG_ENABLED = 0;
	});

	printf("\rframe %02d/%02d: [", screen->frame_number, screen->frame_count);

	// frame 개수를 고려한 percent 계산
	percent = (percent + screen->frame_number * 100) / screen->frame_count;

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

static void do_console(Data *data, char *input_file)
{
	char		output_file[100];			// 출력 이미지 파일 이름 버퍼
	int			frame_number;				// 현재 이미지 frame 번호
	Image*		screen_buffer = NULL;		// bmp파일을 위한 색상정보가 들어가는 배열입니다.

	USE_SCREEN(screen);

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
					// 2 == Y축 회전
					get_rotated_vector(data->primitives[i].vert[j], 2);
					get_rotated_vector(data->primitives[i].norm[j], 2);
				}
			}
		}

		//// -- execute phase --
		PDEBUG("main.c execute phase\n");
		image_reset(screen_buffer);
		screen->frame_number = frame_number;
		do_algorithm(data, screen_buffer, &print_percent);

		//// -- post-step phase --
		PDEBUG("main.c post-step phase\n");

		// output_file 변수에 파일 이름을 집어넣어 줍니다.
		sprintf(output_file, "Result/%s.%04d.bmp", input_file, frame_number + 1);

		// 실제 bmp 파일을 만들어 줍니다.
		//OutputFrameBuffer(screen->xsize, screen->ysize, screen_buffer, output_file);
		image_write(screen_buffer, output_file, IMAGE_NO_FLAGS);
	} // index_x

	screen->frame_number = screen->frame_count;
	print_percent(100.0f);

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
	while ((c = getopt(argc, argv, "Xhc:a:f:s:S:")) != -1)
	{
		if ( LONG_OPTION(longopt, "frame-count") )	c = 'c';
		if ( LONG_OPTION(longopt, "help") )			c = 'h';
		if ( LONG_OPTION(longopt, "file") )			c = 'f';
		if ( LONG_OPTION(longopt, "no-opengl") )	c = 'X';

		switch (c)
		{
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

		case 'X':
			use_opengl = false;
			break;

		case 'f':
			if (input_file[0] != '\0') break;
			printf("set filename %s\n", optarg);
			sprintf(input_file, "%s", optarg);
			break;

		// program will terminated!
		case '?':
		default:
			printf("unknown command\n!");
		case 'h':
			printf(
				"Usage: ./RayTracing.exe [options] [filename]\n"
				"Options:\n"
				"  -h, --help\t\t\t\t"					"Print this message and exit.\n"
				"  -f FILENAME, --file=FILENAME\t\t"	"Set obj filename.\n"
				"  -a (naive|nlog2n|nlongn)\t\t"		"Set search algorithm.\n"
				"  -s (naive|advanced)\t\t\t"			"Set shading algorithm.\n"
				"  -S SCALE\t\t\t\t"					"Set object scale factor\n"
				"  -X\t\t\t\t\t"						"Not use opengl(only for console)\n"
				"  -c COUNT, --frame-count=COUNT\t\t"	"Set frame count(only for console)\n"
				);

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
		PDEBUG("cannot open image file : %s\n", DEFAULT_TEXTURE_FILE);
	// 이미지 쓰기 테스트
	if(image_write(&(data.texture), "texture_save_test.bmp", IMAGE_NO_FLAGS))
		PDEBUG("cannot save image file : %s\n", "texture_save_test.bmp");
	// 이미지 쓰기 테스트
	if(image_write(&(data.texture), "texture_save_test.jpg", IMAGE_NO_FLAGS))
		PDEBUG("cannot save image file : %s\n", "texture_save_test.jpg");

	// 화면 로테이션에 필요한 기본 정보를 집어넣습니다.
	set_rotate(360.0f / screen->frame_count);

	// 결과 출력 폴더를 체크합니다.
	mkdir("Result", NULL);

	// 알고리즘을 수행합니다.
	if (use_opengl)
	{
		glutInit(&argc, argv);
		do_opengl(&data);
	}
	else
	{
		do_console(&data, input_file);
	}

	return 0;
}
