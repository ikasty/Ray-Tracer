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
 * obj ������ �д� �Լ��Դϴ�.
 * ���� ������ http://paulbourke.net/dataformats/obj/ �� �����ϼ���.
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
		
		// �ּ� ó��
		if (op[0] == '#') continue;

		// �������� ���� ������ �о ��ǥ�� �����մϴ�.
		if (strcmp(op, "v") == 0)
		{
			int *vert_count = &data->vert_count;
			static int vert_capacity = 0;
			float x, y, z, w = 1.0;

			// �ʿ��ϴٸ� �迭 ũ�⸦ �ø�
			resize_if_full( (void**)&data->vert, (*vert_count), &vert_capacity, sizeof(data->vert[0]) );

			sscanf(buf + buf_start, "%f %f %f %f", &x, &y, &z, &w);
			// �츮�� w���� ������� ����
			data->vert[*vert_count].x = x;
			data->vert[*vert_count].y = y;
			data->vert[*vert_count].z = z;
			(*vert_count)++;

			continue;
		}

		// �鿡 ���� ������ �о ���� �����ϴ� �������� ID�� �����մϴ�.
		// �������� ID�� ���������� 1�Դϴ�.
		else if (strcmp(op, "f") == 0)
		{
			int read_size;

			int *face_count = &data->face_count;
			static int face_capacity = 0;
			char value[100];
			int v;// , vt, vn;

			int result[3];
			int i = 0;

			// �ʿ��ϴٸ� �迭 ũ�⸦ �ø�
			resize_if_full((void**)&data->face, (*face_count), &face_capacity, sizeof(data->face[0]));

			// ������ %d/%d/%d ����������, �ϴ� %d�� �ִٰ� �����ϰ� �Ѵ�.
			// ���� �ﰢ���� �ƴ� ���� ������, ������ �ﰢ���̶�� �����ϰ� �Ѵ�.
			while ( sscanf(buf + buf_start, "%s%n", value, &read_size) > 0 )
			{
				if (i > 2) break; // TODO: �� ���� ���� ��!

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