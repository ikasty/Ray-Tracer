#include <string.h>
#include <math.h>

#include "settings.h"
#include "include/msl_math.h"
#include "include/type.h"
#include "include/debug-msg.h"
#include "algorithms.h"

enum {X, Y, Z};

void phong_shading(float dest[3], float target_point[3], Primitive prim)
{
	float prim_edge[3][3], vert_point[3][3], norm[3][3];
	float inter_n[3][3], inter_p[3][3];
	int inter_count = 0;

	int i, j;

	COPYTO(vert_point[0], prim.vert0);
	COPYTO(vert_point[1], prim.vert1);
	COPYTO(vert_point[2], prim.vert2);

	// 꼭지점 위에 위치했다면 그냥 해당 꼭지점의 normal 반환
	for (i = 0; i < 3; i++)
	{
		if (is_two_point_equal(target_point, vert_point[i]))
		{
			COPYTO(dest, vert_point[i]);
			return ;
		}
	}

	COPYTO(norm[0], prim.norm0);
	COPYTO(norm[1], prim.norm1);
	COPYTO(norm[2], prim.norm2);

	for (i = 0; i < 3; i++)
	{
		// 노말 벡터 초기화
		dest[i] = 0;
		// 방향 계산
		SUB(prim_edge[i], vert_point[(i + 1) % 3], vert_point[i]);
		// normalize;
		VECTOR_NORMALIZE(norm[i]);
	}

	// 접점을 지나면서 xz 평면에 수평한 평면과, 각 edge와의 접점을 구함.
	for (i = 0; i < 3; i++)
	{
		float u = -1;

		// P + u*D = inter_p 가 될 수 있도록 u를 구함.
		// 만약 xz평면에 평행하지 않다면
		if (prim_edge[i][Y] != 0)
		{
			u = (target_point[Y] - vert_point[i][Y]) / prim_edge[i][Y];
			// 교점이 없으면 쓸모가 없다
			if ( !(0 <= u && u <= 1) ) continue;

			// 적당한 u가 구해졌다면 접점을 확정지음
			for (j = 0; j < 3; j++)
			{
				inter_p[inter_count][j] = vert_point[i][j] + prim_edge[i][j] * u;
				inter_n[inter_count][j] = (1 - u)*norm[i][j] + u*norm[(i + 1) % 3][j];
			}
			VECTOR_NORMALIZE(inter_n[inter_count]);
			inter_count++;
		}
		// edge가 평면 위에 있으면 두 edge의 끝 점을 접점으로 잡고 연산을 끝냄
		else if (fabsf(target_point[Y] - vert_point[i][Y]) < 0.001)
		{
			COPYTO(inter_p[0], vert_point[i]);
			COPYTO(inter_p[1], vert_point[(i + 1) % 3]);
			COPYTO(inter_n[0], norm[i]);
			COPYTO(inter_n[1], norm[(i + 1) % 3]);

			VECTOR_NORMALIZE(inter_n[0]);
			VECTOR_NORMALIZE(inter_n[1]);
			inter_count = 2;
			break;
		}
	}

	// 접점이 2개 나와야 정상임
	if (inter_count == 2)
	{
		float temp[3], temp2[3], temp_length, temp2_length;

		SUB(temp, inter_p[0], target_point);
		temp_length = sqrtf(length_sq(temp));
		SUB(temp2, inter_p[1], target_point);
		temp2_length = sqrtf(length_sq(temp2));

		if (temp_length + temp2_length == 0)
		{
			PDEBUG("phong_shading.c: Unsolved error occured!\n");
		}

		for (i = 0; i < 3; i++)
		{
			dest[i] = (inter_n[0][i] * temp2_length + inter_n[1][i] * temp_length) / (temp_length + temp2_length);
		}
		VECTOR_NORMALIZE(dest);
	}
}
