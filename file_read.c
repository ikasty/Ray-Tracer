#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_read.h"
#include "include/msl_math.h"

#include "include/debug-msg.h"

enum {
	X, Y, Z
};

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

// vertex info
int vert_count = 0, vert_capacity = 0;
Vertex *vert = NULL;

// face info
int face_count = 0, face_capacity = 0;
Triangle *face = NULL;

// normal vector info
int norm_count = 0, norm_capacity = 0;
Normal *norm = NULL;

Primitive *prims = NULL;

static void makePrim()
{
	int i, j;

	if (face_count <= 0) return ;

	// data에 primitives 배열 채워넣기
	prims = (Primitive *)mzalloc(sizeof(Primitive) * face_count);
	for (i = 0; i < face_count; i++)
	{
		Primitive *prim = &prims[i];

		#define FACE_VERT(pnt) (face[i].v[pnt] - 1)
		#define FACE_NORM(pnt) (face[i].vn[pnt] - 1)

		// vertice 정보 복사
		for (j = 0; j < 3; j++) COPYTO(prim->vert[j], vert[ FACE_VERT(j) ].vect);
		prim->prim_id = i;

		// normal vector가 지정되 있다면
		if (use_norm_data)
		{
			float prim_normal[3];

			// normal vector 정보 복사
			for (j = 0; j < 3; j++) COPYTO(prim->norm[j], norm[ FACE_NORM(j) ].norm);
			prim->use_normal = 1;

			// primitive의 시계방향-반시계방향 체크
			{
				// 1. primitive의 normal vector를 구하고
				float temp0[3], temp1[3];

				SUB(temp0, prim->vert[1], prim->vert[0]);
				SUB(temp1, prim->vert[2], prim->vert[0]);
				CROSS(prim_normal, temp0, temp1);
			}

			// 2. 삼각형의 노말 벡터와 꼭지점의 노말 벡터 사이의 각도가 90를 넘어가면 
			if (DOT(norm[ FACE_NORM(0) ].norm, prim_normal) <= 0)
			{
				// 3. 두 번째와 세 번째 꼭지점의 순서를 바꾸어 방향을 바꾼다
				float temp[3];
				memcpy(temp, prim->vert[1], sizeof(float)*3);
				memcpy(prim->vert[1], prim->vert[2], sizeof(float)*3);
				memcpy(prim->vert[2], temp, sizeof(float)*3);
				memcpy(temp, prim->norm[1], sizeof(float) * 3);
				memcpy(prim->norm[1], prim->norm[2], sizeof(float) * 3);
				memcpy(prim->norm[2], temp, sizeof(float) * 3);
			}

		}
	}
}

/**
 * obj 파일을 읽는 함수입니다.
 * 파일 문법은 http://paulbourke.net/dataformats/obj/ 를 참고하세요.
 */
int file_read(FILE* fp, Data *data, float scale)
{
	char buf_orig[100];

	if (fp == NULL)
	{
		printf("file_read.c CRITICAL: NO OBJ FILE!\n");
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
			vert[vert_count].vect[X] = x * scale;
			vert[vert_count].vect[Y] = y * scale;
			vert[vert_count].vect[Z] = z * scale;
			vert_count++;

			continue;
		}
		else if (strcmp(op, "vn") == 0){
			float i, j, k;

			resize_if_full( (void**)&norm, norm_count, &norm_capacity, sizeof(norm[0]) );
			
			sscanf(buf, "%f %f %f", &i, &j, &k);

			norm[norm_count].norm[X] = i;
			norm[norm_count].norm[Y] = j;
			norm[norm_count].norm[Z] = k;
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
				//if (!vn && v <= norm_count) vn = v;

				// 결과에 대입함
				result[0][cnt] = v;
				result[1][cnt] = vt;
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

	makePrim();
	data->primitives = prims;
	data->prim_count = face_count;

	PDEBUG("file_read.c: %d primitives created\n", data->prim_count);

	free(vert);
	free(face);

	return 0;
}
