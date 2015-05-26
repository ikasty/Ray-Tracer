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
		*array = mzalloc(size * (*capacity));
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

// texture info
int tex_count = 0, tex_capacity = 0;
Texture *tex = NULL;

// group info
int current_group_cnt = 0;
Primitive *prims = NULL;

static void makePrim(int use_norm_data)
{
	int i, j;

	if (face_count <= 0) return ;

	// data에 primitives 배열 채워넣기
	if (prims == NULL)
	{
		prims = (Primitive *)mzalloc(sizeof(Primitive) * face_count);
	}
	else
	{
		prims = (Primitive *)realloc(prims, sizeof(Primitive) * face_count);
	}

	#define FACE_VERT(pnt) (face[i].v[pnt] - 1)
	#define FACE_NORM(pnt) (face[i].vn[pnt] - 1)

	for (i = current_group_cnt; i < face_count; i++)
	{
		Primitive *prim = &prims[i];
		float prim_normal[3];

		// 현재 prim id 설정
		prim->prim_id = i;

		// vertice 정보 복사
		for (j = 0; j < 3; j++) COPYTO(prim->vert[j], vert[ FACE_VERT(j) ].vect);

		// primitive의 normal vector를 구한다
		{
			float temp0[3], temp1[3];

			SUB(temp0, prim->vert[1], prim->vert[0]);
			SUB(temp1, prim->vert[2], prim->vert[0]);
			CROSS(prim_normal, temp0, temp1);
		}

		// normal vector가 지정되 있다면
		if (use_norm_data)
		{
			// normal vector 정보 복사
			for (j = 0; j < 3; j++) COPYTO(prim->norm[j], norm[ FACE_NORM(j) ].norm);

			// TODO: 방향 바꾸기는 더 이상 필요가 없을 듯하다
			// 삼각형의 노말 벡터와 꼭지점의 노말 벡터 사이의 각도가 90를 넘어가면
			if (DOT(prim->norm[0], prim_normal) <= 0)
			{
				// 두 번째와 세 번째 꼭지점의 순서를 바꾸어 방향을 바꾼다
				float temp[3];
				memcpy(temp, prim->vert[1], sizeof(float)*3);
				memcpy(prim->vert[1], prim->vert[2], sizeof(float)*3);
				memcpy(prim->vert[2], temp, sizeof(float)*3);
				memcpy(temp, prim->norm[1], sizeof(float) * 3);
				memcpy(prim->norm[1], prim->norm[2], sizeof(float) * 3);
				memcpy(prim->norm[2], temp, sizeof(float) * 3);
			}
		}
		// normal vector 데이터가 없다면 face normal의 평균값들을 이용해서 만들어야 한다
		else
		{
			// normalize한 벡터값을 vertice의 fake_norm에 더한다.
			VECTOR_NORMALIZE(prim_normal);
			for (j = 0; j < 3; j++) ADD(vert[ FACE_VERT(j) ].fake_norm, vert[ FACE_VERT(j) ].fake_norm, prim_normal);
		}
	}

	// normal vector 데이터를 다 저장했다면 빠른 종료
	if (use_norm_data)
	{
		current_group_cnt = face_count;
		return ;
	}
	// normal vector가 없었다면
	else
	{
		// 인접한 primitive의 normal vector의 평균값을 그 vertice에서의 normal vector로 함
		for (i = current_group_cnt; i < face_count; i++)
		{
			Primitive *prim = &prims[i];

			for (j = 0; j < 3; j++)
			{
				COPYTO(prim->norm[j], vert[ FACE_VERT(j) ].fake_norm);
				VECTOR_NORMALIZE(prim->norm[j]);
			}
		}

		// vertice reset
		for (i = current_group_cnt; i < face_count; i++)
		{
			float zero[3] = {0,};

			for (j = 0; j < 3; j++) COPYTO(vert[ FACE_VERT(j) ].fake_norm, zero);
		}
	}

	current_group_cnt = face_count;
}

/**
 * obj 파일을 읽는 함수입니다.
 * 파일 문법은 http://paulbourke.net/dataformats/obj/ 를 참고하세요.
 */
int file_read(FILE* fp, Data *data, float scale)
{

	char buf_orig[100];
	int use_norm_data = 1;

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
		// 텍스쳐 정보
		else if (strcmp(op, "vt") == 0){
			float i, j, k;

			resize_if_full((void**)&tex, tex_count, &tex_capacity, sizeof(tex[0]));

			sscanf(buf, "%f %f %f", &i, &j, &k);

			tex[tex_count].tex[X] = i;
			tex[tex_count].tex[Y] = j;
			tex[tex_count].tex[Z] = k;
			tex_count++;

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
				if (vt < 0) vt += tex_count + 1;
				if (vn < 0) vn += norm_count + 1;

				// normal data 사용 여부 체크
				if (!vn) use_norm_data = 0;

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
					memcpy(face[face_count].vt, result[1], sizeof(result[1]));
					memcpy(face[face_count].vn, result[2], sizeof(result[2]));
					face_count++;

					// 다각형을 삼각형으로 쪼개기 위해 마지막 점을 두 번째 점으로 옮김
					result[0][1] = result[0][2];
					result[1][1] = result[1][2];
					result[2][1] = result[2][2];
				}
			} // while
		} // op "f"
		else if (strcmp(op, "g") == 0)
		{
			// 원래는 그룹명을 읽어서, 같은 그룹은 같이 처리해야 하지만 여기서는 그냥 무시한다
			makePrim(use_norm_data);
			use_norm_data = 1;
		}
		else
		{
			//PDEBUG("unknown operation %s read!\n", op);
		}
	}

	// 마지막 그룹 처리
	if (current_group_cnt != face_count) makePrim(use_norm_data);

	data->primitives = prims;
	data->prim_count = face_count;
	{
		int i;
		for (i = 0; i < data->prim_count; i++)
		{
			Primitive *prim = &data->primitives[i];

			#define FACE_VERT(pnt) (face[i].v[pnt] - 1)
			#define FACE_NORM(pnt) (face[i].vn[pnt] - 1)
			#define FACE_TEX(pnt) (face[i].vt[pnt] - 1)

			prim->vert[0][X] = vert[ FACE_VERT(0) ].vect[X];
			prim->vert[0][Y] = vert[ FACE_VERT(0) ].vect[Y];
			prim->vert[0][Z] = vert[ FACE_VERT(0) ].vect[Z];

			prim->vert[1][X] = vert[ FACE_VERT(1) ].vect[X];
			prim->vert[1][Y] = vert[ FACE_VERT(1) ].vect[Y];
			prim->vert[1][Z] = vert[ FACE_VERT(1) ].vect[Z];
			
			prim->vert[2][X] = vert[ FACE_VERT(2) ].vect[X];
			prim->vert[2][Y] = vert[ FACE_VERT(2) ].vect[Y];
			prim->vert[2][Z] = vert[ FACE_VERT(2) ].vect[Z];

			prim->prim_id = i;


			// texture coordi가 지정되 있다면
			if (FACE_TEX(0) >= 0)
			{
				prim->text[0][X] = tex[FACE_TEX(0)].tex[X];
				prim->text[0][Y] = tex[FACE_TEX(0)].tex[Y];
				prim->text[0][Z] = tex[FACE_TEX(0)].tex[Z];

				prim->text[1][X] = tex[FACE_TEX(1)].tex[X];
				prim->text[1][Y] = tex[FACE_TEX(1)].tex[Y];
				prim->text[1][Z] = tex[FACE_TEX(1)].tex[Z];

				prim->text[2][X] = tex[FACE_TEX(2)].tex[X];
				prim->text[2][Y] = tex[FACE_TEX(2)].tex[Y];
				prim->text[2][Z] = tex[FACE_TEX(2)].tex[Z];

				prim->use_texture = 1;
			}

			// normal vector가 지정되 있다면
			if (FACE_NORM(0) >= 0)
			{
				float temp0[3], temp1[3];
				float prim_normal[3];

				prim->norm[0][X] = norm[ FACE_NORM(0) ].norm[X];
				prim->norm[0][Y] = norm[ FACE_NORM(0) ].norm[Y];
				prim->norm[0][Z] = norm[ FACE_NORM(0) ].norm[Z];

				prim->norm[1][X] = norm[ FACE_NORM(1) ].norm[X];
				prim->norm[1][Y] = norm[ FACE_NORM(1) ].norm[Y];
				prim->norm[1][Z] = norm[ FACE_NORM(1) ].norm[Z];
				
				prim->norm[2][X] = norm[ FACE_NORM(2) ].norm[X];
				prim->norm[2][Y] = norm[ FACE_NORM(2) ].norm[Y];
				prim->norm[2][Z] = norm[ FACE_NORM(2) ].norm[Z];

				prim->use_normal = 1;

				// primitive의 시계방향-반시계방향 체크
				// 1. primitive의 normal vector를 구하고
				SUB(temp0, prim->vert[1], prim->vert[0]);
				SUB(temp1, prim->vert[2], prim->vert[0]);
				CROSS(prim_normal, temp0, temp1);

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
					memcpy(temp, prim->text[1], sizeof(float) * 3);
					memcpy(prim->text[1], prim->text[2], sizeof(float) * 3);
					memcpy(prim->text[2], temp, sizeof(float) * 3);
				}

			}
		}
	}

	PDEBUG("file_read.c: %d primitives created\n", data->prim_count);

	free(vert);
	free(face);
	free(norm);

	return 0;
}
