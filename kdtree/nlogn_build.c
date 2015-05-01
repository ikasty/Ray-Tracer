#include <float.h>
#include <stdlib.h>
#include "nlogn_build.h"
#include "kdtree_common.h"

#include "kdtree_type.h"
#include "bbox.h"

#include "../include/debug-msg.h"

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

	for (i = 0; i < nEdges; i++){
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
static void buildTree(KDAccelTree *kdtree, KDAccelNode *current_node, BBox *nodeBounds,
			   int total_prim_counts, int depth, BoundEdge *edge_buffer, int nEdges, int badRefines)
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

	int bestNAbove = 0, bestNBelow = 0, bestSide = BELOW;
	int nBelow[3], nPlanar[3], nAbove[3];
	BoundEdge bestPlane = {0};
	BoundEdge *leftEdges, *rightEdges;
	int nLeftEdges, nRightEdges;
	int *primNums;

	int i;

	// 자식 노드를 위한 공간 확보
	allocChild(kdtree);

	// 종료 조건에 다다르면 리프 노드로 생성
	if (total_prim_counts <= kdtree->maxPrims || depth == 0)
	{
		primNums = (int*)malloc(total_prim_counts * sizeof(int));
		get_prim_nums_from_edges(primNums, kdtree->nPrims, edge_buffer, nEdges);
		initLeaf(kdtree, current_node, primNums, total_prim_counts);
		free(primNums);
		return;
	}
	
	{
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
			float minBound = nodeBounds->faaBounds[0][curPlane.axis];
			float maxBound = nodeBounds->faaBounds[1][curPlane.axis];
			int nEndOfCurPlane = 0, nPlanarOfCurPlane = 0, nStartOfCurPlane = 0;

			// 현재 평면에 닿아 있는 edge들을 분류함
			while (i < nEdges && edge_buffer[i].axis == curPlane.axis && edge_buffer[i].t == curPlane.t)
			{
				switch (edge_buffer[i++].e_type)
				{
				case END:	nEndOfCurPlane++;	break;
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
					else			bestNAbove += nCurP;

					DEBUG_ONLY(if (bestCost < 0)
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
	}
	if (bestCost > oldCost) badRefines++;

	// 분할할 만한 적당한 위치가 없으면 리프 노드 생성
	if ((bestCost > 4.f * oldCost && total_prim_counts < 16) || bestPlane.axis == -1 || badRefines >= 3)
	{
		primNums = (int*)malloc(total_prim_counts * sizeof(int));
		get_prim_nums_from_edges(primNums, kdtree->nPrims, edge_buffer, nEdges);
		initLeaf(kdtree, current_node, primNums, total_prim_counts);
		free(primNums);
		return;
	}

	// bestPlane을 바탕으로 edges를 left, right 두 그룹으로 나누는 과정
	{
		BoundEdge *leftOnlys, *rightOnlys, *bothLefts, *bothRights;
		int *locationsOfPrims;
		int nLO = 0, nRO = 0, nBL = 0, nBR = 0;

		leftOnlys 		= (BoundEdge *)malloc(nEdges * sizeof(BoundEdge));
		rightOnlys	 	= (BoundEdge *)malloc(nEdges * sizeof(BoundEdge));
		bothLefts 		= (BoundEdge *)malloc(nEdges * sizeof(BoundEdge));
		bothRights 		= (BoundEdge *)malloc(nEdges * sizeof(BoundEdge));
		locationsOfPrims	= (int *)malloc(kdtree->nPrims * sizeof(int));

		// edge에 따라 primitive가 어느 쪽에 속하는지 결정		
		for (i = 0; i < kdtree->nPrims; i++)
			locationsOfPrims[i] = BOTH;
		for (i = 0; i < nEdges; i++)
		{
			if (edge_buffer[i].axis == bestPlane.axis)
			{
				switch (edge_buffer[i].e_type)
				{
				case END:
					if (edge_buffer[i].t <= bestPlane.t) 
						locationsOfPrims[edge_buffer[i].primNum] = BELOW;
					break;
				case START:
					if (edge_buffer[i].t >= bestPlane.t) 
						locationsOfPrims[edge_buffer[i].primNum] = ABOVE;
					break;
				case PLANAR:
					if (edge_buffer[i].t < bestPlane.t || (edge_buffer[i].t == bestPlane.t && bestSide == BELOW))
						locationsOfPrims[edge_buffer[i].primNum] = BELOW;
					if (edge_buffer[i].t > bestPlane.t || (edge_buffer[i].t == bestPlane.t && bestSide == ABOVE))
						locationsOfPrims[edge_buffer[i].primNum] = ABOVE;
					break;
				}
			}			
		}

		// 중간 배열에 분류한 edge를 담음
		for (i = 0; i < nEdges; i++)
		{
			switch(locationsOfPrims[edge_buffer[i].primNum])
			{
			case BELOW:	
				leftOnlys[nLO++] = edge_buffer[i]; 
				break;
			case ABOVE:
				rightOnlys[nRO++] = edge_buffer[i];
				break;
			case BOTH:
				if(edge_buffer[i].axis != bestPlane.axis)
				{
					bothLefts[nBL++] = edge_buffer[i];
					bothRights[nBR++] = edge_buffer[i];
				}
				else if(edge_buffer[i].e_type == START)
				{
					BoundEdge newEnd;
					init_bound_edge(&newEnd, bestPlane.t, edge_buffer[i].primNum, END, bestPlane.axis);
					
					bothLefts[nBL++] = edge_buffer[i];
					bothLefts[nBL++] = newEnd;
				} 
				else if(edge_buffer[i].e_type == END)
				{
					BoundEdge newStart;
					init_bound_edge(&newStart, bestPlane.t, edge_buffer[i].primNum, START, bestPlane.axis);

					bothRights[nBR++] = newStart;
					bothRights[nBR++] = edge_buffer[i];
				} 
				break;
			}
		}

		// both left, right 배열 정렬함 O(√n(log√n))
		qsort(bothLefts, nBL, sizeof(BoundEdge), compare_bound);
		qsort(bothRights, nBR, sizeof(BoundEdge), compare_bound);

		// 배열들을 합함
		nLeftEdges	= nLO + nBL;
		nRightEdges	= nRO + nBR;
		leftEdges	= (BoundEdge*)malloc(nLeftEdges * sizeof(BoundEdge));
		rightEdges	= (BoundEdge*)malloc(nRightEdges * sizeof(BoundEdge));		
		merge_bound(leftEdges, leftOnlys, bothLefts, nLO, nBL);
		merge_bound(rightEdges, rightOnlys, bothRights, nRO, nBR);
		
		free(leftOnlys);
		free(rightOnlys);
		free(bothLefts);
		free(bothRights);
		free(locationsOfPrims);
	}

	PDEBUG("nlogn buildTree depth %d, cost %f, below %d, above %d\n", 10 - depth, bestCost, nLeftEdges, nRightEdges);

	// 재귀적으로 자식 노드 초기화
	bbox_below = *nodeBounds;
	bbox_above = *nodeBounds;
	bbox_below.faaBounds[0][bestPlane.axis] = bestPlane.t;
	bbox_above.faaBounds[1][bestPlane.axis] = bestPlane.t;

	below_child = &kdtree->nodes[ kdtree->nextFreeNodes ];
	buildTree(kdtree, below_child, &bbox_below,
		bestNBelow, depth-1, leftEdges, nLeftEdges, badRefines);

	above_child = &kdtree->nodes[ kdtree->nextFreeNodes ];
	buildTree(kdtree, above_child, &bbox_above,
		bestNAbove, depth-1, rightEdges, nRightEdges, badRefines);

	initInterior(current_node, above_child, below_child, bestPlane.axis, bestPlane.t);
	
	free(leftEdges);
	free(rightEdges);
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
	buildTree(kdtree, &kdtree->nodes[0], &kdtree->bounds, kdtree->nPrims, kdtree->maxDepth, edge_buffer, nEdges, 0);
	
	// kdtree 구축에 사용한 공간 해제
	free(primBounds);
	free(edge_buffer);
	free(prim_indexes);
}

void nlogn_accel_build(Data *data)
{
	KDAccelTree *kdtree = (KDAccelTree *)mzalloc(sizeof(KDAccelTree));
	data->accel_struct = (void *)kdtree;

	kdtree->isectCost = 80;
	kdtree->traversalCost = 1;
	kdtree->emptyBonus = 0.5f;
	kdtree->maxPrims = 1;
	kdtree->maxDepth = 10;
	kdtree->nPrims = data->prim_count;

	kdtree->primitives = (Primitive *)malloc(sizeof(data->primitives[0]) * data->prim_count);
	memcpy(kdtree->primitives, data->primitives, sizeof(data->primitives[0]) * data->prim_count);

	// 지금 트리가 가진 노드의 개수는 0개입니다.
	kdtree->nextFreeNodes = 0;
	kdtree->nAllocednodes = 512;
	kdtree->nodes = (KDAccelNode *)mzalloc(sizeof(KDAccelNode) * 512);
	
	initTree(kdtree);
}
