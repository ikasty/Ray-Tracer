#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_read.h"

static void resize_if_full(void **array, int curr, int *capacity, int size)
{
	if (*capacity == 0)
	{
		*capacity = 100;
		*array = malloc(size * (*capacity));
	}
	else if (curr == *capacity)
	{
		*array = realloc(*array, size * (*capacity) * 2);
		*capacity *= 2;
	}
}

/**
 * obj 파일을 읽는 함수입니다.
 * 파일 문법은 http://paulbourke.net/dataformats/obj/ 를 참고하세요.
 */
int file_read(FILE* fp, struct Data *data)
{
	char buf[100];
	int buf_start = 0;

	if (fp == NULL)
		return -1;

	while (fgets(buf, 99, fp))
	{
		char op[10];
		
		sscanf(buf, "%s%n", op, &buf_start);
		
		// 주석 처리
		if (op[0] == '#') continue;

		// 꼭지점에 대한 정보를 읽어서 좌표를 저장합니다.
		if (strcmp(op, "v") == 0)
		{
			int *vert_count = &data->vert_count;
			static int vert_capacity = 0;
			float x, y, z, w = 1.0;

			// 필요하다면 배열 크기를 늘림
			resize_if_full( (void**)&data->vert, (*vert_count), &vert_capacity, sizeof(data->vert[0]) );

			sscanf(buf + buf_start, "%f %f %f %f", &x, &y, &z, &w);
			// 우리는 w값을 사용하지 않음
			data->vert[*vert_count].x = x;
			data->vert[*vert_count].y = y;
			data->vert[*vert_count].z = z;
			(*vert_count)++;

			continue;
		}

		// 면에 대한 정보를 읽어서 면을 구성하는 꼭지점의 ID를 저장합니다.
		// 꼭지점의 ID는 위에서부터 1입니다.
		else if (strcmp(op, "f") == 0)
		{
			int read_size;

			int *face_count = &data->face_count;
			static int face_capacity = 0;
			char value[100];
			int v;// , vt, vn;

			int result[3];
			int i = 0;

			// 필요하다면 배열 크기를 늘림
			resize_if_full((void**)&data->face, (*face_count), &face_capacity, sizeof(data->face[0]));

			// 원래는 %d/%d/%d 포맷이지만, 일단 %d만 있다고 가정하고 한다.
			// 또한 삼각형이 아닐 수도 있지만, 무조건 삼각형이라고 가정하고 한다.
			while ( sscanf(buf + buf_start, "%s%n", value, &read_size) > 0 )
			{
				if (i > 2) break; // TODO: 이 문장 없앨 것!

				buf_start += read_size;
				sscanf(value, "%d", &v);
				result[i++] = v;
			}

			data->face[*face_count].v1 = result[0];
			data->face[*face_count].v2 = result[1];
			data->face[*face_count].v3 = result[2];
			(*face_count)++;

		}
	}

	return 0;
}