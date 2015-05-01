#include <float.h>
#include <stdlib.h>
#include "nlog2n_build.h"
#include "kdtree_common.h"

#include "kdtree_type.h"
#include "bbox.h"

#include "../include/debug-msg.h"

#define PLANAR_TRY_TWICE
#define MAX_LEVEL 20

// edge 후보를 저장할 버퍼 변수
static BoundEdge *edge_buffer[3];

static int compare_bound(const void *a, const void *b)
{
	BoundEdge *l = (BoundEdge *)a, *r = (BoundEdge *)b;

	if (l->t == r->t)
		return (l->e_type) < (r->e_type)? 1: -1;
	else
		return (l->t) > (r->t)? 1: -1;
};

// build tree 함수는 tree node마다 불러와지므로 
// 인테리어 노드와 리프노드를 구별해야 한다.
static void buildTree(KDAccelTree *kdtree, KDAccelNode *current_node, BBox *nodeBounds,
			BBox *allPrimBounds, int *prim_indexes, int total_prim_counts, int depth,
			int *below_prims, int *above_prims, int badRefines)
{
	// kdtree의 자식 노드 변수
	KDAccelNode *below_child, *above_child;
	// 자식 노드의 boundary box
	BBox bbox_below, bbox_above;

	// 제한 cost
	float oldCost = (float)kdtree->isectCost * total_prim_counts;
	// total SA의 역수값
	float invTotalSA = 1.f / get_surface_of_bbox(nodeBounds);
	// 최적 cost
	float bestCost = FLT_MAX;
	// diagonal vector
	float d[3] = {
		nodeBounds->faaBounds[1][0] - nodeBounds->faaBounds[0][0],
		nodeBounds->faaBounds[1][1] - nodeBounds->faaBounds[0][1],
		nodeBounds->faaBounds[1][2] - nodeBounds->faaBounds[0][2]
	};

	int nCount = 0, below_count = 0, above_count = 0;
	int bestAxis = -1, axis, bestSide;
	float bestSplit = -1.0f;
	int retries = 0;

	int i;

	// 자식 노드를 위한 공간 확보
	allocChild(kdtree);

	// 종료 조건에 다다르면 리프 노드로 생성
	if (total_prim_counts <= kdtree->maxPrims || depth == 0)
	{
		// 단말 노드로 등록
		initLeaf(kdtree, current_node, prim_indexes, total_prim_counts);
		return;
	}
	// 내부 노드를 초기화 하고 재귀 진행

	// 어느 축으로 자를지 결정함. 가장 길쭉한 축부터 시도
	axis = find_axis_of_maximum_extent(nodeBounds);
	while (retries < 3)
	{
		int nBelow = 0, nAbove = total_prim_counts, nPlanar = 0;
		nCount = 0;

		// 축을 기준으로 edge를 초기화
		for (i = 0; i < total_prim_counts; i++)
		{
			// 이 노드에 속한 primitive들을 돌면서 edge를 찾음
			int pn = prim_indexes[i];
			BBox bbox = allPrimBounds[pn];

			// 해당 primitive의 속성에 따라 split 후보 생성
			if (bbox.faaBounds[0][axis] == bbox.faaBounds[1][axis])
			{
				init_bound_edge(&edge_buffer[axis][nCount++], bbox.faaBounds[0][axis], pn, PLANAR, axis);
			}
			else
			{
				init_bound_edge(&edge_buffer[axis][nCount++], bbox.faaBounds[0][axis], pn, START, axis);
				init_bound_edge(&edge_buffer[axis][nCount++], bbox.faaBounds[1][axis], pn, END, axis);
			}
		}
		qsort(edge_buffer[axis], nCount, sizeof(BoundEdge), compare_bound);
		
		// 베스트를 찾기 위해 모든 코스트를 계산함
		i = 0;
		while (i < nCount)
		{
			BoundEdge curPlane = edge_buffer[axis][i];
			float minBound = nodeBounds->faaBounds[0][curPlane.axis];
			float maxBound = nodeBounds->faaBounds[1][curPlane.axis];
			int start_count = 0, end_count = 0, planar_count = 0;

			// 현재 split 후보에 걸치고 있는 primitive의 타입을 체크한다
			while (	(i < nCount) && (curPlane.t == edge_buffer[axis][i].t) )
			{
				if (edge_buffer[axis][i].e_type == START)	start_count++;
				if (edge_buffer[axis][i].e_type == END)		end_count++;
				if (edge_buffer[axis][i].e_type == PLANAR)	planar_count++;
				i++;
			}

			// split 후보 위에 있는 primitive 개수 갱신
			nAbove -= (end_count + planar_count);
			nPlanar = planar_count;

			if (minBound < curPlane.t && curPlane.t < maxBound)
			{
				float cost;
				int side;

				// 이 노드 범위 내에 있는 edge를 위한 cost 계산
				int otherAxis0 = (axis + 1) % 3, otherAxis1 = (axis + 2) % 3;
				float pBelow = 2 * (
					(d[otherAxis0] * d[otherAxis1]) + (curPlane.t - minBound) * (d[otherAxis0] + d[otherAxis1])
				) * invTotalSA;
				float pAbove = 2 * (
					(d[otherAxis0] * d[otherAxis1]) + (maxBound - curPlane.t) * (d[otherAxis0] + d[otherAxis1])
				) * invTotalSA;

				cost = getCost(kdtree, nBelow, nPlanar, nAbove, pBelow, pAbove, &side);

				// 최소 코스트라면 갱신
				if (cost < bestCost)
				{
					bestAxis = axis;
					bestSplit = curPlane.t;
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

			// split 후보 아래에 있는 primitive 개수 갱신
			nBelow += (start_count + planar_count);
			nPlanar = 0;
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
			if (bestCost > oldCost) badRefines++;

			// split 후보를 찾지 못했거나, split한 결과가 손해라면
			if (bestAxis == -1 ||
			    badRefines >= 3 ||
			   (bestCost > 4.f * oldCost && total_prim_counts < 16))
			{
				// 단말 노드로 등록
				initLeaf(kdtree, current_node, prim_indexes, total_prim_counts);
				return;
			}
			break;
		}
	}

	// 분할에 대해 프리미티브를 분류함
	for (i = 0; i < nCount; i++)
	{
		BoundEdge *edge = &edge_buffer[bestAxis][i];

		// split 지점 밑에 있는 start 또는 planar
		if ( (edge->t < bestSplit) && (edge->e_type == START || edge->e_type == PLANAR) )
		{
			below_prims[below_count++] = edge->primNum;
		}
		// split 지점에 있는 planar는 side에 따라 분류
		if ( (edge->t == bestSplit) && (edge->e_type == PLANAR) )
		{
			if (bestSide == BELOW)
				below_prims[below_count++] = edge->primNum;
			else
				above_prims[above_count++] = edge->primNum;
		}
		// split 지점 위에 있는 end 또는 planar
		if ( (edge->t > bestSplit) && (edge->e_type == END || edge->e_type == PLANAR))
		{
			above_prims[above_count++] = edge->primNum;
		}
	}

	PDEBUG("nlog2n buildTree depth %d, cost %f, below %d, above %d\n", MAX_LEVEL - depth, bestCost, below_count, above_count);

	// 재귀적으로 자식 노드 초기화
	bbox_below = *nodeBounds;
	bbox_above = *nodeBounds;
	bbox_below.faaBounds[0][bestAxis] = bestSplit;
	bbox_above.faaBounds[1][bestAxis] = bestSplit;

	// 아래 노드 탐색
	below_child = &kdtree->nodes[ kdtree->nextFreeNodes ];
	buildTree(kdtree, below_child, &bbox_below,
		allPrimBounds, below_prims, below_count, depth - 1,
		below_prims, above_prims + total_prim_counts, badRefines);

	// 위 노드 탐색
	above_child = &kdtree->nodes[ kdtree->nextFreeNodes ];
	buildTree(kdtree, above_child, &bbox_above,
		allPrimBounds, above_prims, above_count, depth - 1,
		below_prims, above_prims + total_prim_counts, badRefines);

	// 현재 노드를 부모 노드로 설정
	initInterior(current_node, above_child, below_child, bestAxis, bestSplit);
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

	// edge buffer에 필요한 공간 할당
	for (i = 0; i < 3; i++)
	{
		edge_buffer[i] = (BoundEdge *)malloc(sizeof(BoundEdge) * prims_count * 2);
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
	buildTree(kdtree, &kdtree->nodes[0], &kdtree->bounds, primBounds, prim_indexes, kdtree->nPrims,
		kdtree->maxDepth, prims0, prims1, 0);
	
	// kdtree 구축에 사용한 공간 해제
	free(primBounds);
	for (i = 0; i < 3; i++)
	{
		free(edge_buffer[i]);
	}
	free(prims0);
	free(prims1);
	free(prim_indexes);
}

void nlog2n_accel_build(Data *data)
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
