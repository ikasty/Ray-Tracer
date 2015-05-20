
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

#include <float.h>
#include <stdlib.h>
#include "binning_build.h"
#include "kdtree_common.h"

#include "kdtree_type.h"
#include "bbox.h"

#include "include/debug-msg.h"

#define MAX_LEVEL 20
#define BIN_COUNT 64

static int choose_bin(float bounds, float base, float top, float length)
{
	int i = 0;
	while ( (bounds > (base += length)) && (i < BIN_COUNT - 1) ) i++;
	
	return i;
}

// build tree 함수는 tree node마다 불러와지므로 
// 인테리어 노드와 리프노드를 구별해야 한다.
static void buildTree(KDAccelTree *kdtree, int current_node_idx, BBox *nodeBounds,
			BBox *allPrimBounds, int *prim_indexes, int total_prim_counts, int depth,
			int *below_prims, int *above_prims, int badRefines, float oldCost)
{// TODO: allPrimBounds를 전역 변수로 뺄 것
	// kdtree의 자식 노드 변수
	int below_child_idx, above_child_idx;
	// 자식 노드의 boundary box
	BBox bbox_below, bbox_above;

	// total SA의 역수값
	float invTotalSA = 1.f / get_surface_of_bbox(nodeBounds);
	// 최적 cost
	float bestCost = oldCost;
	// diagonal vector
	float d[3] = {
		nodeBounds->faaBounds[1][0] - nodeBounds->faaBounds[0][0],
		nodeBounds->faaBounds[1][1] - nodeBounds->faaBounds[0][1],
		nodeBounds->faaBounds[1][2] - nodeBounds->faaBounds[0][2]
	};

	int below_count = 0, above_count = 0;
	int bestAxis = -1, axis = -1, bestSide = BELOW;
	float bestSplit = -1.0f;
	int retries = 0;

	int i;

	// 자식 노드를 위한 공간 확보
	allocChild(kdtree);

	// 종료 조건에 다다르면 리프 노드로 생성
	if (total_prim_counts <= kdtree->maxPrims || depth == 0)
	{
		// 단말 노드로 등록
		initLeaf(kdtree, current_node_idx, prim_indexes, total_prim_counts);
		return;
	}
	// 내부 노드를 초기화 하고 재귀 진행

	// 어느 축으로 자를지 결정함. 가장 길쭉한 축부터 시도
	axis = find_axis_of_maximum_extent(nodeBounds);
	while (retries < 3)
	{
		float minBound = nodeBounds->faaBounds[0][axis];
		float maxBound = nodeBounds->faaBounds[1][axis];
		float bin_length = (maxBound - minBound) / BIN_COUNT;

		// 카운팅을 위한 event 저장 배열
		int Levent[BIN_COUNT] = {0,}, Hevent[BIN_COUNT] = {0,};
		for (i = 0; i < total_prim_counts; i++)
		{
			int prim_num = prim_indexes[i];
			BBox bbox = allPrimBounds[prim_num];
			int bin;

			// Low event count
			bin = choose_bin(bbox.faaBounds[0][axis], minBound, maxBound, bin_length);
			Levent[bin]++;

			// High event count
			bin = choose_bin(bbox.faaBounds[1][axis], minBound, maxBound, bin_length);
			Hevent[bin]++;
		}

		// Low event는 뒤에서부터 누적
		for (i = BIN_COUNT - 2; i >= 0; i--)
			Levent[i] += Levent[i + 1];

		// High event는 앞에서부터 누적
		for (i = 1; i < BIN_COUNT; i++)
			Hevent[i] += Hevent[i - 1];

		// 베스트를 찾기 위해 모든 코스트를 계산함
		for (i = 1; i < BIN_COUNT; i++) // TODO: nCount -> edge_count로 변경할 것
		{
			float split_t = minBound + bin_length * i;
			int nBelow, nAbove, nBoth, nPlanar;
			float cost;
			int side;

			// split 후보 위에 있는 primitive 개수 갱신
			nAbove = Hevent[i - 1];
			nBelow = Levent[i];
			nBoth = total_prim_counts - Levent[i] - Hevent[i - 1];

			// 플래너는 나올 수가 없다
			nPlanar = 0;

			nAbove += nBoth; nBelow += nBoth;

			// 이 노드 범위 내에 있는 edge를 위한 cost 계산
			int otherAxis0 = (axis + 1) % 3, otherAxis1 = (axis + 2) % 3;
			float pBelow = 2 * (
				(d[otherAxis0] * d[otherAxis1]) + (split_t - minBound) * (d[otherAxis0] + d[otherAxis1])
			) * invTotalSA;
			float pAbove = 2 * (
				(d[otherAxis0] * d[otherAxis1]) + (maxBound - split_t) * (d[otherAxis0] + d[otherAxis1])
			) * invTotalSA;

			cost = getCost(kdtree, nBelow, nPlanar, nAbove, pBelow, pAbove, &side);

			// 최소 코스트라면 갱신
			if (cost < bestCost)
			{
				bestAxis = axis;
				bestSplit = split_t;
				bestCost = cost;
				bestSide = side;

				DEBUG_ONLY(if (bestCost < 0)
				{
					PDEBUG("WARN: cost %f, nBelow %d, pBelow %.2f, nAbove %d, pAbove %.2f\n",
						bestCost, nBelow, pBelow, nAbove, pAbove);
					PAUSE;
				});
			}
		}

		// 분할할 만한 적당한 위치가 없으면 축을 바꿔서 재시도
		if (bestAxis == -1 && retries < 2)
		{
			retries++;
			axis = (axis + 1) % 3;
		}
		else
		{
			// split이 손해인 경우 체크
			if (bestCost >= oldCost) badRefines++;

			// split 후보를 찾지 못했거나, split한 결과가 손해라면
			if (bestAxis == -1 ||
			    badRefines >= 3 ||
			   (bestCost > 4.f * oldCost && total_prim_counts < 16))
			{
				// 단말 노드로 등록
				initLeaf(kdtree, current_node_idx, prim_indexes, total_prim_counts);
				return;
			}
			break;
		}
	}

	// 분할에 대해 프리미티브를 분류함
	for (i = 0; i < total_prim_counts; i++)
	{
		int prim_num = prim_indexes[i];
		BBox bbox = allPrimBounds[prim_num];
		float left = bbox.faaBounds[0][axis], right = bbox.faaBounds[1][axis];

		// split 지점 밑에 있는 start
		if ( left < bestSplit )
		{
			below_prims[below_count++] = prim_num;
		}
		// split 지점에 있는 planar는 side에 따라 분류
		if ( (left == bestSplit) && (left == right) )
		{
			if (bestSide == BELOW)
				below_prims[below_count++] = prim_num;
			else
				above_prims[above_count++] = prim_num;
		}
		// split 지점 위에 있는 end
		if ( right > bestSplit )
		{
			above_prims[above_count++] = prim_num;
		}
	}

	PDEBUG("binning depth %d, cost %f at %f, total %d, below %d, above %d\n", MAX_LEVEL - depth, bestCost, bestSplit, total_prim_counts, below_count, above_count);

	// 재귀적으로 자식 노드 초기화
	bbox_below = *nodeBounds;
	bbox_above = *nodeBounds;
	bbox_below.faaBounds[1][bestAxis] = bestSplit;
	bbox_above.faaBounds[0][bestAxis] = bestSplit;

	// 아래 노드 탐색
	below_child_idx = kdtree->nextFreeNodes;
	buildTree(kdtree, below_child_idx, &bbox_below,
		allPrimBounds, below_prims, below_count, depth - 1,
		below_prims, above_prims + total_prim_counts, badRefines, bestCost);

	// 위 노드 탐색
	above_child_idx = kdtree->nextFreeNodes;
	buildTree(kdtree, above_child_idx, &bbox_above,
		allPrimBounds, above_prims, above_count, depth - 1,
		below_prims, above_prims + total_prim_counts, badRefines, bestCost);

	// 현재 노드를 부모 노드로 설정
	initInterior(kdtree, current_node_idx, above_child_idx, below_child_idx, bestAxis, bestSplit);
}

static void initTree(KDAccelTree *kdtree)
{
	/* int icost, int tcost, float ebonus, int maxp, int md */
	BBox *primBounds;
	int *prim_indexes, *prims0, *prims1;
	int prims_count = kdtree->nPrims;

	int i;

	// kdtree의 전체 bound 계산 및 각 primitive의 bound를 계산해 놓음
	primBounds = (BBox *)malloc(sizeof(BBox) * prims_count);
	for (i = 0; i < prims_count; i++)
	{
		BBox b = make_bbox_by_triangle(kdtree->primitives[i]);
		kdtree->bounds = union_bbox_and_bbox(kdtree->bounds, b);
		primBounds[i] = b;
	}

	// primitive가 저장될 버퍼 할당
	prims0 = (int *)mzalloc(sizeof(int) * prims_count);
	prims1 = (int *)mzalloc(sizeof(int) * prims_count * (kdtree->maxDepth + 1));

	// kdtree 구축을 prim_indexes(노드에 들어있는 프리미티브의 인덱스 모음) 초기화 
	prim_indexes = (int *)malloc(sizeof(int) * prims_count);
	for (i = 0; i < prims_count; i++)
	{
		prim_indexes[i] = i;
	}
	
	// kdtree 구축을 위한 재귀문 실행
	buildTree(kdtree, 0, &kdtree->bounds, primBounds, prim_indexes, kdtree->nPrims,
		kdtree->maxDepth, prims0, prims1, 0, FLT_MAX);
	
	// kdtree 구축에 사용한 공간 해제
	free(primBounds);
	free(prims0);
	free(prims1);
	free(prim_indexes);
}

void binning_accel_build(Data *data)
{
	KDAccelTree *kdtree = (KDAccelTree *)mzalloc(sizeof(KDAccelTree));
	data->accel_struct = (void *)kdtree;

	kdtree->isectCost = 80;
	kdtree->traversalCost = 1;
	kdtree->emptyBonus = 0.5f;
	kdtree->maxPrims = 1;
	kdtree->maxDepth = MAX_LEVEL;
	kdtree->nPrims = data->prim_count;

	kdtree->primitives = (Primitive *)malloc(sizeof(data->primitives[0]) * data->prim_count);
	memcpy(kdtree->primitives, data->primitives, sizeof(data->primitives[0]) * data->prim_count);

	// 지금 트리가 가진 노드의 개수는 0개입니다.
	kdtree->nextFreeNodes = 0;
	kdtree->nAllocednodes = 512;
	kdtree->nodes = (KDAccelNode *)mzalloc(sizeof(KDAccelNode) * 512);
	
	initTree(kdtree);
}

// TODO: binning_clear_accel 수정할 것
void binning_clear_accel(Data *data)
{
	int i;
	KDAccelTree *kdtree = (KDAccelTree *)data->accel_struct;

	if (kdtree == NULL) return ;

	free(kdtree->primitives);

	for (i = 0; i < kdtree->nextFreeNodes; i++)
	{
		KDAccelNode *node = (KDAccelNode *)&kdtree->nodes[i];
		free(node->primitives);
	}

	free(kdtree->nodes);
	data->accel_struct = zfree(kdtree);
}
