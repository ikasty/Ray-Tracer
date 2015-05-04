#include <string.h>
#include <math.h>

#include "settings.h"
#include "include/msl_math.h"
#include "include/type.h"
#include "algorithms.h"

void phong_shading(float dest[3], float point[3], Primitive prim)
{
	float inter_n[3][3], inter_p[3][3];
	float temp[3], temp2[3], temp_length, temp2_length;
	int i, j, inter_count = 0;
	float d[3][3], p[3][3], n[3][3];

	// 꼭지점 위에 위치했다면 그냥 해당 꼭지점의 normal 반환
	if (is_two_point_equal(point, prim.vert0))
	{
		SUBST(dest, prim.norm0);
		return;
	}
	if (is_two_point_equal(point, prim.vert1))
	{
		SUBST(dest, prim.norm1);
		return;
	}
	if (is_two_point_equal(point, prim.vert2))
	{
		SUBST(dest, prim.norm2);
		return;
	}

	SUBST(p[0], prim.vert0);
	SUBST(p[1], prim.vert1);
	SUBST(p[2], prim.vert2);
	SUBST(n[0], prim.norm0);
	SUBST(n[1], prim.norm1);
	SUBST(n[2], prim.norm2);
	for (i = 0; i < 3; i++){
		// 노말 벡터 초기화
		dest[i] = 0;
		// 방향 계산
		SUB(d[i], p[(i + 1) % 3], p[i]);
		// normalize;
		VECTOR_NORMALIZE(n[i]);
	}

	// 접점을 지나면서 xz 평면에 수평한 평면과, 각 edge와의 접점을 구함.
	for (i = 0; i < 3; i++){
		float u = -1;
		int flag = 0;

		// P + u*D = inter_p 가 될 수 있도록 u를 구함.
		if (d[i][1] != 0){
			u = (point[1] - p[i][1]) / d[i][1];
			if (0 <= u && u <= 1){
				flag = 1;
			}
		}
		else if (fabsf(point[1] - p[i][1]) < 0.001){
			flag = 1;
		}

		// 적당한 u가 구해졌다면 접점을 확정지음
		if (flag && u != -1){
			for (j = 0; j < 3; j++){
				inter_p[inter_count][j] = p[i][j] + d[i][j] * u;
				inter_n[inter_count][j] = (1 - u)*n[i][j] + u*n[(i + 1) % 3][j];
			}
			VECTOR_NORMALIZE(inter_n[inter_count]);
			inter_count++;
		}
		// edge가 평면 위에 있으면 두 edge의 끝 점을 접점으로 잡고 연산을 끝냄
		else if (flag && u == -1){
			for (j = 0; j < 3; j++){
				inter_p[0][j] = p[i][j];
				inter_n[0][j] = n[i][j];
				inter_p[1][j] = p[(i + 1) % 3][j];
				inter_n[1][j] = n[(i + 1) % 3][j];
			}
			VECTOR_NORMALIZE(inter_n[0]);
			VECTOR_NORMALIZE(inter_n[1]);
			inter_count = 2;
			break;
		}
	}

	// 접점이 2개 나와야 정상임
	if (inter_count == 2){
		SUB(temp, inter_p[0], point);
		temp_length = sqrtf(length_sq(temp));
		SUB(temp2, inter_p[1], point);
		temp2_length = sqrtf(length_sq(temp2));

		if (temp_length + temp2_length == 0){
			printf("???");
		}

		for (i = 0; i < 3; i++)
		{
			dest[i] = (inter_n[0][i] * temp2_length + inter_n[1][i] * temp_length) / (temp_length + temp2_length);
		}
		VECTOR_NORMALIZE(dest);
	}
}
