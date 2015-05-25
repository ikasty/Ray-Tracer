#include <float.h>
#include <stdlib.h>
#include "exclusion_build.h"
#include "kdtree_common.h"

#include "kdtree_type.h"
#include "bbox.h"

#define PERCENT 0.15

#include "include/debug-msg.h"

static BoundEdge *below_onlys, *above_onlys, *both_for_below, *both_for_above;
static int *locationsOfPrims;

static int compare_bound(const void *a, const void *b)
{
	BoundEdge *l = (BoundEdge *)a, *r = (BoundEdge *)b;

	// 1. t값으로 정렬
	if (l->t != r->t)
		return (l->t) > (r->t) ? 1 : -1;

	// 2. axis로 정렬
	if (l->axis != r->axis)
		return (l->axis) > (r->axis) ? 1 : -1;

	// 3. type로 정렬
	return (l->e_type) > (r->e_type) ? 1 : -1;
}

static void merge_bound(BoundEdge* result, BoundEdge* e1, BoundEdge* e2, int nE1, int nE2)
{
	int rCount = 0, i = 0, j = 0;
  
	while (i < nE1 || j < nE2)
	{
		if (i < nE1 && j < nE2)
		{
			if (compare_bound(&e1[i], &e2[j]) < 0)
				result[rCount++] = e1[i++];
			else
				result[rCount++] = e2[j++];
		}
		else if(i >= nE1 && j < nE2)
			result[rCount++] = e2[j++];
		else if(i < nE1 && j >= nE2)
			result[rCount++] = e1[i++];
	}
}

static void get_prim_nums_from_edges(int* prims, int nPrimsMax, BoundEdge* edges, int nEdges)
{
	int i, nPrims = 0;
	int *checked_prims = (int *)calloc(nPrimsMax, sizeof(int));

	for (i = 0; i < nEdges; i++)
	{
		if (!checked_prims[edges[i].primNum])
		{
			prims[nPrims++] = edges[i].primNum;
			checked_prims[edges[i].primNum] = 1;
		}		
	}

	free(checked_prims);
}

// build tree 함수는 tree node마다 불러와지므로 
// 인테리어 노드와 리프노드를 구별해야 한다.
static void buildTree(KDAccelTree *kdtree, int current_node_idx, BBox *nodeBounds,
			   int total_prim_counts, int depth, BoundEdge *edge_buffer, int nEdges, int badRefines)
{
	// kdtree의 자식 노드 변수
	int below_child_idx, above_child_idx;
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
	// exclusion 범위
	float exclusion[3][2] = {
		{nodeBounds->faaBounds[0][0] + d[0] * PERCENT, nodeBounds->faaBounds[1][0] - d[0] * PERCENT},
		{nodeBounds->faaBounds[0][1] + d[1] * PERCENT, nodeBounds->faaBounds[1][1] - d[1] * PERCENT},
		{nodeBounds->faaBounds[0][2] + d[2] * PERCENT, nodeBounds->faaBounds[1][2] - d[2] * PERCENT}
	};

	int bestNAbove = 0, bestNBelow = 0, bestSide = BELOW;
	int nBelow[3], nPlanar[3], nAbove[3];
	BoundEdge bestPlane = {0};
	BoundEdge *belowEdges, *aboveEdges;
	int nBelowEdges = 0, nAboveEdges = 0;
	int *primNums;

	int i;

	// 자식 노드를 위한 공간 확보
	allocChild(kdtree);

	// 종료 조건에 다다르면 리프 노드로 생성
	if (total_prim_counts <= kdtree->maxPrims || depth == 0)
	{
		primNums = (int*)malloc(total_prim_counts * sizeof(int));
		get_prim_nums_from_edges(primNums, kdtree->nPrims, edge_buffer, nEdges);
		initLeaf(kdtree, current_node_idx, primNums, total_prim_counts);
		free(primNums);
		return;
	}

	// bestPlane 초기화
	bestPlane.axis = -1;

	// 평면으로 나누어진 영역에 속한 prim 개수 초기화 
	for (i = 0; i < 3; i++)
	{
		nAbove[i] = total_prim_counts;
		nPlanar[i] = 0;
		nBelow[i] = 0;
	}

	// edge로 평면을 만들어 나눴을 때 cost가 최소가 되는 평면 구함
	i = 0;
	while (i < nEdges)
	{
		BoundEdge curPlane = edge_buffer[i];
		float minBound = exclusion[curPlane.axis][0];
		float maxBound = exclusion[curPlane.axis][1];
		int nEndOfCurPlane = 0, nPlanarOfCurPlane = 0, nStartOfCurPlane = 0;

		// 현재 평면에 닿아 있는 edge들을 분류함
		while ( (i < nEdges) && (edge_buffer[i].axis == curPlane.axis) && (edge_buffer[i].t == curPlane.t) )
		{
			switch (edge_buffer[i++].e_type)
			{
			case END:		nEndOfCurPlane++;		break;
			case PLANAR: 	nPlanarOfCurPlane++; 	break;
			case START: 	nStartOfCurPlane++; 	break;
			}
		}
		nAbove[curPlane.axis] -= (nEndOfCurPlane + nPlanarOfCurPlane);
		nPlanar[curPlane.axis] = nPlanarOfCurPlane;

		// 코스트 계산
		if (minBound < curPlane.t && curPlane.t < maxBound)
		{
			float cost;
			int side;

			// 이 노드 범위 내에 있는 edge를 위한 cost 계산
			int otherAxis0 = (curPlane.axis + 1) % 3, otherAxis1 = (curPlane.axis + 2) % 3;
			float pBelow = 2 * (
				(d[otherAxis0] * d[otherAxis1]) + (curPlane.t - minBound) * (d[otherAxis0] + d[otherAxis1])
			) * invTotalSA;
			float pAbove = 2 * (
				(d[otherAxis0] * d[otherAxis1]) + (maxBound - curPlane.t) * (d[otherAxis0] + d[otherAxis1])
			) * invTotalSA;

			int nCurB = nBelow[curPlane.axis];
			int nCurA = nAbove[curPlane.axis];
			int nCurP = nPlanar[curPlane.axis];

			cost = getCost(kdtree, nCurB, nCurP, nCurA, pBelow, pAbove, &side);

			// 최소 코스트라면 갱신
			if (cost < bestCost)
			{
				bestPlane = curPlane;
				bestCost = cost;
				bestSide = side;
				bestNBelow = nCurB;
				bestNAbove = nCurA;

				if (side == BELOW)	bestNBelow += nCurP;
				else				bestNAbove += nCurP;

				DEBUG_ONLY(if (bestCost < 0 || nBelow < 0 || nAbove < 0)
				{
					PDEBUG("WARN: cost %f, nBelow %d, pBelow %.2f, nAbove %d, pAbove %.2f\n",
						bestCost, nCurB, pBelow, nCurA, pAbove);
					PAUSE;
				});
			}
		}			

		// 다음 loop를 위한 변수 조정 
		nPlanar[curPlane.axis] = 0;
		nBelow[curPlane.axis] += (nPlanarOfCurPlane + nStartOfCurPlane);
	} // while

	if (bestCost > oldCost) badRefines++;

	// 분할할 만한 적당한 위치가 없으면 리프 노드 생성
	if ((bestCost > 4.f * oldCost && total_prim_counts < 16) || bestPlane.axis == -1 || badRefines >= 3)
	{
		primNums = (int*)malloc(total_prim_counts * sizeof(int));
		get_prim_nums_from_edges(primNums, kdtree->nPrims, edge_buffer, nEdges);
		initLeaf(kdtree, current_node_idx, primNums, total_prim_counts);
		free(primNums);
		return;
	}

	// bestPlane을 바탕으로 edges를 left, right 두 그룹으로 나누는 과정
	{
		int nB = 0, nA = 0, nBB = 0, nBA = 0;

		// edge에 따라 primitive가 어느 쪽에 속하는지 결정
		for (i = 0; i < nEdges; i++)
			locationsOfPrims[edge_buffer[i].primNum] = BOTH;
		for (i = 0; i < nEdges; i++)
		{
			if (edge_buffer[i].axis == bestPlane.axis)
			{
				switch (edge_buffer[i].e_type)
				{
				case END:
					if		(edge_buffer[i].t <= bestPlane.t)
						locationsOfPrims[edge_buffer[i].primNum] = BELOW;
					break;

				case START:
					if		(edge_buffer[i].t >= bestPlane.t)
						locationsOfPrims[edge_buffer[i].primNum] = ABOVE;
					break;

				case PLANAR:
					if		(edge_buffer[i].t < bestPlane.t)
						locationsOfPrims[edge_buffer[i].primNum] = BELOW;
					else if (edge_buffer[i].t > bestPlane.t)
						locationsOfPrims[edge_buffer[i].primNum] = ABOVE;
					else if (edge_buffer[i].t == bestPlane.t)
						locationsOfPrims[edge_buffer[i].primNum] = bestSide;
					break;
				}
			}
		}

		// 중간 배열에 분류한 edge를 담음
		for (i = 0; i < nEdges; i++)
		{
			switch (locationsOfPrims[edge_buffer[i].primNum])
			{
			case BELOW:	
				below_onlys[nB++] = edge_buffer[i];
				break;
			case ABOVE:
				above_onlys[nA++] = edge_buffer[i];
				break;
			case BOTH:
				both_for_below[nBB++] = edge_buffer[i];
				both_for_above[nBA++] = edge_buffer[i];

				// 우선은 both 상황 시 split point 고려하지 않고 무조건 둘 다 넣는 것으로 함
/*				if(edge_buffer[i].axis != bestPlane.axis)
				{
					both_for_below[nBB++] = edge_buffer[i];
					both_for_above[nBA++] = edge_buffer[i];
				}
				else if(edge_buffer[i].e_type == START)
				{
					BoundEdge newEnd;
					init_bound_edge(&newEnd, bestPlane.t, edge_buffer[i].primNum, END, bestPlane.axis);
					
					both_for_below[nBB++] = edge_buffer[i];
					both_for_below[nBB++] = newEnd;
				} 
				else if(edge_buffer[i].e_type == END)
				{
					BoundEdge newStart;
					init_bound_edge(&newStart, bestPlane.t, edge_buffer[i].primNum, START, bestPlane.axis);

					both_for_above[nBA++] = newStart;
					both_for_above[nBA++] = edge_buffer[i];
				}
*/
				break;
			}
		}

		// both left, right 배열 정렬함 O(√n(log√n))
		qsort(both_for_below, nBB, sizeof(BoundEdge), compare_bound);
		qsort(both_for_above, nBA, sizeof(BoundEdge), compare_bound);

		// below와 above의 edge candidate 개수 계산
		nBelowEdges = nB + nBB;
		nAboveEdges = nA + nBA;

		// 배열들을 합함
		belowEdges = (BoundEdge *)malloc(sizeof(BoundEdge) * nBelowEdges);
		aboveEdges = (BoundEdge *)malloc(sizeof(BoundEdge) * nAboveEdges);
		merge_bound(belowEdges, below_onlys, both_for_below, nB, nBB);
		merge_bound(aboveEdges, above_onlys, both_for_above, nA, nBA);
	};

	PDEBUG("exclusion buildTree depth %d, cost %f, below %d, above %d\n", 10 - depth, bestCost, nBelowEdges, nAboveEdges);

	// 재귀적으로 자식 노드 초기화
	bbox_below = *nodeBounds;
	bbox_above = *nodeBounds;
	bbox_below.faaBounds[1][bestPlane.axis] = bestPlane.t;
	bbox_above.faaBounds[0][bestPlane.axis] = bestPlane.t;

	below_child_idx = kdtree->nextFreeNodes;
	buildTree(kdtree, below_child_idx, &bbox_below, bestNBelow, depth - 1, belowEdges, nBelowEdges, badRefines);

	above_child_idx = kdtree->nextFreeNodes;
	buildTree(kdtree, above_child_idx, &bbox_above, bestNAbove, depth - 1, aboveEdges, nAboveEdges, badRefines);

	initInterior(kdtree, current_node_idx, above_child_idx, below_child_idx, bestPlane.axis, bestPlane.t);

	free(belowEdges);
	free(aboveEdges);
}

static void initTree(KDAccelTree *kdtree)
{
	// edge 후보를 저장할 버퍼 변수
	BoundEdge *edge_buffer;
	BBox *primBounds;
	int *prim_indexes;
	int prims_count = kdtree->nPrims;

	int i, nEdges;

	// kdtree의 전체 bound 계산 및 각 primitive의 bound를 계산해 놓음
	primBounds = (BBox *)malloc(sizeof(BBox) * prims_count);
	for (i = 0; i < prims_count; i++)
	{
		BBox b = make_bbox_by_triangle(kdtree->primitives[i]);
		kdtree->bounds = union_bbox_and_bbox(kdtree->bounds, b);
		primBounds[i] = b;
	}

	// kdtree 구축을 prim_indexes(노드에 들어있는 프리미티브의 인덱스 모음) 초기화 
	prim_indexes = (int *)malloc(sizeof(int) * prims_count);
	for (i = 0; i < prims_count; i++){
		prim_indexes[i] = i;
	}

	// edge_buffer 공간 할당
	edge_buffer = (BoundEdge *)malloc(sizeof(BoundEdge) * prims_count * 2 * 3);

	// primitive 분류용 공간 할당
	locationsOfPrims	= (int *)malloc(sizeof(int) * prims_count);
	below_onlys 		= (BoundEdge *)malloc(sizeof(BoundEdge) * prims_count * 2 * 3);
	above_onlys	 		= (BoundEdge *)malloc(sizeof(BoundEdge) * prims_count * 2 * 3);
	both_for_below 		= (BoundEdge *)malloc(sizeof(BoundEdge) * prims_count * 2 * 3);
	both_for_above 		= (BoundEdge *)malloc(sizeof(BoundEdge) * prims_count * 2 * 3);

	// edge buffer 초기화
	nEdges = 0;
	for (i = 0; i < prims_count; i++)
	{
		int pn = prim_indexes[i];
		BBox bbox = primBounds[pn];
		int axis;

		for (axis = 0; axis < 3; axis++)
		{
			float bbox_min = bbox.faaBounds[0][axis], bbox_max = bbox.faaBounds[1][axis];

			if (bbox_min == bbox_max)
			{
				init_bound_edge(&edge_buffer[nEdges++], bbox_min, pn, PLANAR, axis);
			}
			else
			{
				init_bound_edge(&edge_buffer[nEdges++], bbox_min, pn, START, axis);
				init_bound_edge(&edge_buffer[nEdges++], bbox_max, pn, END, axis);
			}
		}
	}
	qsort(edge_buffer, nEdges, sizeof(BoundEdge), compare_bound);

	// kdtree 구축을 위한 재귀문 실행
	buildTree(kdtree, 0, &kdtree->bounds, kdtree->nPrims, kdtree->maxDepth, edge_buffer, nEdges, 0);
	
	// kdtree 구축에 사용한 공간 해제
	free(primBounds);
	free(edge_buffer);
	free(prim_indexes);

	free(locationsOfPrims);
	free(below_onlys);
	free(above_onlys);
	free(both_for_below);
	free(both_for_above);
}

void exclusion_accel_build(Data *data)
{
	KDAccelTree *kdtree = (KDAccelTree *)mzalloc(sizeof(KDAccelTree));
	data->accel_struct = (void *)kdtree;

	kdtree->isectCost = 80;
	kdtree->traversalCost = 1;
	kdtree->emptyBonus = 0.5f;
	kdtree->maxPrims = 16;
	kdtree->maxDepth = 30;
	kdtree->nPrims = data->prim_count;

	kdtree->primitives = (Primitive *)malloc(sizeof(data->primitives[0]) * data->prim_count);
	memcpy(kdtree->primitives, data->primitives, sizeof(data->primitives[0]) * data->prim_count);

	// 지금 트리가 가진 노드의 개수는 0개입니다.
	kdtree->nextFreeNodes = 0;
	kdtree->nAllocednodes = 512;
	kdtree->nodes = (KDAccelNode *)mzalloc(sizeof(KDAccelNode) * 512);
	
	initTree(kdtree);

	DEBUG_ONLY(leaf_info_print());
}
