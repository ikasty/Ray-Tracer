#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_read.h"
#include "include/msl_math.h"

#include "include/debug-msg.h"


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
	int data[3] = {0,}, index = 0;
	int value = 0;

	while (*c) {
		switch (*c)
		{
		case '-':
			boolean = -1;
			break;
		case '/':
		case ' ':
		case '\t':
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
	data[index++] = value * boolean;

	*v = data[0];
	*vt = data[1];
	*vn = data[2];
}

/**
 * obj 파일을 읽는 함수입니다.
 * 파일 문법은 http://paulbourke.net/dataformats/obj/ 를 참고하세요.
 */
int file_read(FILE* fp, Data *data, float scale)
{
	// vertex info
	int vert_count = 0, vert_capacity = 0;
	Vertex *vert = NULL;

	// face info
	int face_count = 0, face_capacity = 0;
	Triangle *face = NULL;

	// normal vector info
	int norm_count = 0, norm_capacity = 0;
	Vertex *norm = NULL;

	char buf_orig[100];

	if (fp == NULL)
	{
		printf("CRITICAL: NO OBJ FILE!\n");
		return -1;
	}

	PDEBUG("file_read.c: file_read start\n");

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
			float x, y, z, w = 1.0;

			// 필요하다면 배열 크기를 늘림
			resize_if_full( (void**)&vert, vert_count, &vert_capacity, sizeof(vert[0]) );

			sscanf(buf, "%f %f %f %f", &x, &y, &z, &w);

			// 우리는 w값을 사용하지 않음
			vert[vert_count].vect[0] = x * scale;
			vert[vert_count].vect[1] = y * scale;
			vert[vert_count].vect[2] = z * scale;

			vert[vert_count].norm[0] = 0;
			vert[vert_count].norm[1] = 0;
			vert[vert_count].norm[2] = 0;
			vert_count++;

			continue;
		}
		else if (strcmp(op, "vn") == 0){
			float x, y, z, w = 1.0;

			resize_if_full( (void**)&norm, norm_count, &norm_capacity, sizeof(norm[0]) );
			
			sscanf(buf, "%f %f %f %f", &x, &y, &z, &w);

			norm[norm_count].vect[0] = x;
			norm[norm_count].vect[1] = y;
			norm[norm_count].vect[2] = z;
			norm_count++;

			continue;
		}
		// 면에 대한 정보를 읽어서 면을 구성하는 꼭지점의 ID를 저장합니다.
		// 꼭지점의 ID는 위에서부터 1입니다.
		else if (strcmp(op, "f") == 0)
		{
			char face_buf[100];

			// result[type][vert]
			// type: 0 - vertex, 1 - texture, 2 - normal vector
			int result[3][3];
			int cnt = 0;

			// 가능한 포맷은 %d, %d/%d, %d/%d/%d, %d//%d 임
			// 각각 v, v/vt, v/vt/vn, v//vn을 의미함
			while ( sscanf(buf, "%s%n", face_buf, &read_size) > 0 )
			{
				int v = 0, vt = 0, vn = 0;
				buf += read_size;

				// face_buf에서 각각의 값을 읽어옴
				face_read(face_buf, &v, &vt, &vn);

				// relative accessing 지원
				if (v < 0) v += vert_count + 1;
				if (vn < 0) vn += norm_count + 1;

				// vn값이 없으면 v랑 같은 인덱스를 사용함
				if (!vn && v <= norm_count) vn = v;

				// 결과에 대입함
				result[0][cnt] = v;
				result[2][cnt] = vn;

				if (cnt < 2)
				{
					cnt++;
				}
				else
				{
					// 필요하다면 배열 크기를 늘림
					resize_if_full((void**)&face, face_count, &face_capacity, sizeof(face[0]));

					// 삼각형 데이터 넣기
					memcpy(face[face_count].v,  result[0], sizeof(result[0]));
					memcpy(face[face_count].vn, result[2], sizeof(result[2]));
					face_count++;

					// 다각형을 삼각형으로 쪼개기 위해 마지막 점을 두 번째 점으로 옮김
					result[0][1] = result[0][2];
					result[1][1] = result[1][2];
					result[2][1] = result[2][2];
				}
			} // while
		} // op "f"
		else
		{
			//PDEBUG("unknown operation %s read!\n", op);
		}
	}

	PDEBUG("file_read.c: file read successfully finished!\n");

	// data에 primitives 배열 채워넣기
	// TODO: vert0, vert1, vert2를 배열로 바꿔보자
	data->primitives = (Primitive *)mzalloc(sizeof(Primitive) * face_count);
	data->prim_count = face_count;
	{
		int i;
		for (i = 0; i < data->prim_count; i++)
		{
			Primitive *prim = &data->primitives[i];
			float temp0[3], temp1[3], temp2[3], temp3[3];

			prim->vert0[0] = vert[ face[i].v[0] - 1 ].vect[0];
			prim->vert0[1] = vert[ face[i].v[0] - 1 ].vect[1];
			prim->vert0[2] = vert[ face[i].v[0] - 1 ].vect[2];

			prim->vert1[0] = vert[ face[i].v[1] - 1 ].vect[0];
			prim->vert1[1] = vert[ face[i].v[1] - 1 ].vect[1];
			prim->vert1[2] = vert[ face[i].v[1] - 1 ].vect[2];
			
			prim->vert2[0] = vert[ face[i].v[2] - 1 ].vect[0];
			prim->vert2[1] = vert[ face[i].v[2] - 1 ].vect[1];
			prim->vert2[2] = vert[ face[i].v[2] - 1 ].vect[2];

			prim->prim_id = i;

			//normal vector와 비교해서 결과 값을 고치는 코드 추가
			SUB(temp0, prim->vert1, prim->vert0);
			SUB(temp1, prim->vert2, prim->vert0);
			CROSS(temp2, temp0, temp1);

			// vertex와 인접한 면의 normal vector 합산
			ADD(vert[face[i].v[0] - 1].norm, vert[face[i].v[0] - 1].norm, temp2);
			ADD(vert[face[i].v[1] - 1].norm, vert[face[i].v[1] - 1].norm, temp2);
			ADD(vert[face[i].v[2] - 1].norm, vert[face[i].v[2] - 1].norm, temp2);

			if (face[i].vn[0] && norm[ face[i].vn[0] - 1 ].vect[0] != 0)
			{				
				// 삼각형의 노말 벡터와 꼭지점의 노말 벡터 사이의 각도가 90를 넘어가면 
				if (DOT(norm[ face[i].vn[0] - 1 ].vect, temp2) <= 0)
				{
					memcpy(temp3, prim->vert1, sizeof(float)*3);
					memcpy(prim->vert1, prim->vert2, sizeof(float)*3);
					memcpy(prim->vert2, temp3, sizeof(float)*3);
				}
				else
				{
					int a = 0;
					a+= 1;
				}
			}
		}
		// 각 버텍스마다 접하는 노멀 벡터 값을 모두 더했으므로, 이 값을 프리미티브에 추가
		for (i = 0; i < data->prim_count; i++)
		{
			Primitive *prim = &data->primitives[i];
			SUBST(prim->norm0, vert[face[i].v[0] - 1].norm);
			SUBST(prim->norm1, vert[face[i].v[1] - 1].norm);
			SUBST(prim->norm2, vert[face[i].v[2] - 1].norm);
		}
	}

	PDEBUG("file_read.c: %d primitives created\n", data->prim_count);

	free(vert);
	free(face);

	return 0;
}
