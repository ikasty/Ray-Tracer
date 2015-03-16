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

static void face_read(char *buf, int *v, int *vt, int *vn)
{
	char *c = buf;
	int boolean = 1;
	int data[3], index = 0;
	int value = 0;

	while (*c) {
		switch (*c)
		{
		case '-':
			boolean = -1;
			break;
		case '/':
			data[index++] = value * boolean;
			boolean = 1;
			value = 0;
			break;
		default:
			// error check
			if ((*c) < '0' || (*c) > '9') return ;

			value *= 10;
			value += (*c) - '0';
		}
		c++;
	} // while

	*v = data[0];
	*vt = data[1];
	*vn = data[2];
}

/**
 * obj 파일을 읽는 함수입니다.
 * 파일 문법은 http://paulbourke.net/dataformats/obj/ 를 참고하세요.
 */
int file_read(FILE* fp, struct Data *data)
{
	char buf_orig[100];

	if (fp == NULL)
	{
		printf("CRITICAL: NO OBJ FILE!");
		return -1;
	}

	while (fgets(buf_orig, 99, fp))
	{
		char *buf = buf_orig;
		int read_size = 0;
		char op[10];

		sscanf(buf, "%s%n", op, &read_size);
		buf += read_size;

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

			sscanf(buf, "%f %f %f %f", &x, &y, &z, &w);
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
			int *face_count = &data->face_count;
			static int face_capacity = 0;
			char face_buf[100];
			int v, vt, vn;

			int result[3];
			int cnt = 0;

			// 필요하다면 배열 크기를 늘림
			resize_if_full((void**)&data->face, (*face_count), &face_capacity, sizeof(data->face[0]));

			// 가능한 포맷은 %d, %d/%d, %d/%d/%d, %d//%d 임
			// 각각 v, v/vt, v/vt/vn, v//vn을 의미함
			while ( sscanf(buf, "%s%n", face_buf, &read_size) > 0 )
			{
				buf += read_size;

				// face_buf에서 각각의 값을 읽어옴
				face_read(face_buf, &v, &vt, &vn);
				// 하지만 현재는 v값만 사용함
				result[cnt] = v;

				// relative accessing 지원
				if (result[cnt] < 0) result[cnt] += data->vert_count;

				if (cnt < 2)
				{
					cnt++;
				}
				else
				{
					// 삼각형 데이터 넣기
					data->face[*face_count].v1 = result[0];
					data->face[*face_count].v2 = result[1];
					data->face[*face_count].v3 = result[2];
					(*face_count)++;
					
					// 다각형을 삼각형으로 쪼개기 위해 넣음
					result[1] = result[2];
				}
			} // while
		} // op "f"
	}

	return 0;
}