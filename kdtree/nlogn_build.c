#include <float.h>
#include <stdlib.h>
#include "nlogn_build.h"
#include "kdtree_common.h"

#include "kdtree_type.h"
#include "bbox.h"
#include "boundedge.h"

#include "../include/debug-msg.h"

static int compare_bound(const void *a, const void *b)
{
	BoundEdge *l = (BoundEdge *)a, *r = (BoundEdge *)b;

	if (l->t == r->t)
		return (l->e_type) > (r->e_type)? 1: -1;
	else
		return (l->t) > (r->t)? 1: -1;
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
static void buildTree(KDAccelTree *kdtree, int nodeNum, BBox *nodeBounds,
			   int nPrimitives, int depth, BoundEdge *edges, int nEdges, int badRefines)
{
	int i;
	float bestCost = FLT_MAX;
	int bestNAbove = 0, bestNBelow = 0, bestSide = BELOW;
	float oldCost = (float)kdtree->isectCost * nPrimitives;
	float invTotalSA = 1.f / get_surface_of_bbox(nodeBounds);
	float d[3] = {
		nodeBounds->faaBounds[1][0] - nodeBounds->faaBounds[0][0],
		nodeBounds->faaBounds[1][1] - nodeBounds->faaBounds[0][1],
		nodeBounds->faaBounds[1][2] - nodeBounds->faaBounds[0][2]
	};
	BBox bounds0, bounds1;
	int nBelow[3], nPlanar[3], nAbove[3];
	BoundEdge bestPlane = {0};
	BoundEdge *leftEdges, *rightEdges;
	int nLeftEdges, nRightEdges;
	int *primNums;
	KDAccelNode *below_child, *above_child;

	// 노드 배열로 부터 비어있는 다음 노드를 구함
	// 가득 찼으면 공간 늘려서 새로 할당
	if (kdtree->nextFreeNodes == kdtree->nAllocednodes)
	{
		int *alloc_size = &kdtree->nAllocednodes;

		if ((*alloc_size) == 0)
		{
			*alloc_size = 512;
			kdtree->nodes = (KDAccelNode *)malloc(sizeof(KDAccelNode) * (*alloc_size));
		}
		else
		{
			*alloc_size *= 2;
			kdtree->nodes = (KDAccelNode *)realloc(kdtree->nodes, sizeof(KDAccelNode) * (*alloc_size));
		}
	}
	kdtree->nextFreeNodes++;

	// 종료 조건에 다다르면 리프 노드로 생성
	if (nPrimitives <= kdtree->maxPrims || depth == 0)
	{
		primNums = (int*)malloc(nPrimitives * sizeof(int));
		get_prim_nums_from_edges(primNums, kdtree->nPrims, edges, nEdges);
		initLeaf(kdtree, &kdtree->nodes[nodeNum], primNums, nPrimitives);
		free(primNums);
		return;
	}
	
	//bestPlane 찾기 
	{
		// bestPlane 초기화
		bestPlane.axis = -1;

		// 평면으로 나누어진 영역에 속한 prim 개수 초기화 
		for (i = 0; i < 3; i++)
		{
			nAbove[i] = nPrimitives;
			nPlanar[i] = 0;
			nBelow[i] = 0;
		}

		// edge로 평면을 만들어 나눴을 때 cost가 최소가 되는 평면 구함
		i = 0;
		while (i < nEdges)
		{
			BoundEdge curPlane = edges[i];
			int nEndOfCurPlane = 0, nPlanarOfCurPlane = 0, nStartOfCurPlane = 0;
			float minBound = nodeBounds->faaBounds[0][curPlane.axis];
			float maxBound = nodeBounds->faaBounds[1][curPlane.axis];

			// 현재 평면에 닿아 있는 edge들을 분류함 
			while (i < nEdges && edges[i].axis == curPlane.axis && edges[i].t == curPlane.t)
			{
				switch(edges[i++].e_type)
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
	if ((bestCost > 4.f * oldCost && nPrimitives < 16) || bestPlane.axis == -1 || badRefines >= 3)
	{
		primNums = (int*)malloc(nPrimitives * sizeof(int));
		get_prim_nums_from_edges(primNums, kdtree->nPrims, edges, nEdges);
		initLeaf(kdtree, &kdtree->nodes[nodeNum], primNums, nPrimitives);
		free(primNums);
		return;
	}

	// bestPlane을 바탕으로 edges를 left, right 두 그룹으로 나누는 과정
	{
		BoundEdge *leftOnlys, *rightOnlys, *bothLefts, *bothRights;
		int *locationsOfPrims;
		int nLO = 0, nRO = 0, nBL = 0, nBR = 0;

		leftOnlys 		= (BoundEdge*)malloc(nEdges * sizeof(BoundEdge));
		rightOnlys	 	= (BoundEdge*)malloc(nEdges * sizeof(BoundEdge));
		bothLefts 		= (BoundEdge*)malloc(nEdges * sizeof(BoundEdge));
		bothRights 		= (BoundEdge*)malloc(nEdges * sizeof(BoundEdge));
		locationsOfPrims	= (int*)malloc(kdtree->nPrims * sizeof(int));

		// edge에 따라 primitive가 어느 쪽에 속하는지 결정		
		for (i = 0; i < kdtree->nPrims; i++)
			locationsOfPrims[i] = BOTH;
		for (i = 0; i < nEdges; i++)
		{
			if (edges[i].axis == bestPlane.axis)
			{
				switch (edges[i].e_type)
				{
				case END:
					if (edges[i].t <= bestPlane.t) 
						locationsOfPrims[edges[i].primNum] = BELOW;
					break;
				case START:
					if (edges[i].t >= bestPlane.t) 
						locationsOfPrims[edges[i].primNum] = ABOVE;
					break;
				case PLANAR:
					if (edges[i].t < bestPlane.t || (edges[i].t == bestPlane.t && bestSide==BELOW))
						locationsOfPrims[edges[i].primNum] = BELOW;
					if (edges[i].t > bestPlane.t || (edges[i].t == bestPlane.t && bestSide==ABOVE))
						locationsOfPrims[edges[i].primNum] = ABOVE;
					break;
				}
			}			
		}

		// 중간 배열에 분류한 edge를 담음
		for (i = 0; i < nEdges; i++)
		{
			switch(locationsOfPrims[edges[i].primNum])
			{
			case BELOW:	
				leftOnlys[nLO++] = edges[i]; 
				break;
			case ABOVE:
				rightOnlys[nRO++] = edges[i];
				break;
			case BOTH:
				if(edges[i].axis != bestPlane.axis)
				{
					bothLefts[nBL++] = edges[i];
					bothRights[nBR++] = edges[i];
				}
				else if(edges[i].e_type == START)
				{
					BoundEdge newEnd;
					init_nlogn_bound_edge(&newEnd, bestPlane.t, edges[i].primNum, END, bestPlane.axis);
					
					bothLefts[nBL++] = edges[i];
					bothLefts[nBL++] = newEnd;
				} 
				else if(edges[i].e_type == END)
				{
					BoundEdge newStart;
					init_nlogn_bound_edge(&newStart, bestPlane.t, edges[i].primNum, START, bestPlane.axis);

					bothRights[nBR++] = newStart;
					bothRights[nBR++] = edges[i];
				} 
				break;
			}
		}

		// both left, right 배열 정렬함 O(√n(log√n))
		qsort(bothLefts, nBL, sizeof(BoundEdge), compare_bound);
		qsort(bothRights, nBR, sizeof(BoundEdge), compare_bound);

		// 배열들을 합함
		nLeftEdges 	= nLO + nBL;
		nRightEdges = nRO + nBR;
		leftEdges 	= (BoundEdge*)malloc(nLeftEdges * sizeof(BoundEdge));
		rightEdges 	= (BoundEdge*)malloc(nRightEdges * sizeof(BoundEdge));		
		merge_bound(leftEdges, leftOnlys, bothLefts, nLO, nBL);
		merge_bound(rightEdges, rightOnlys, bothRights, nRO, nBR);
		
		free(leftOnlys);
		free(rightOnlys);
		free(bothLefts);
		free(bothRights);
		free(locationsOfPrims);
	}

	PDEBUG("nlog2n buildTree depth %d, cost %f, below %d, above %d\n", 10 - depth, bestCost, nLeftEdges, nRightEdges);

	// 재귀적으로 자식 노드 초기화
	bounds0 = *nodeBounds;
	bounds1 = *nodeBounds;
	bounds0.faaBounds[0][bestPlane.axis] = bestPlane.t;
	bounds1.faaBounds[1][bestPlane.axis] = bestPlane.t;

	below_child = &kdtree->nodes[ kdtree->nextFreeNodes ];
	buildTree(kdtree, nodeNum+1, &bounds0,
		bestNBelow, depth-1, leftEdges, nLeftEdges, badRefines);

	above_child = &kdtree->nodes[ kdtree->nextFreeNodes ];
	buildTree(kdtree, kdtree->nextFreeNodes, &bounds1,
		bestNAbove, depth-1, rightEdges, nRightEdges, badRefines);

	initInterior(&kdtree->nodes[nodeNum], above_child, below_child, bestPlane.axis, bestPlane.t);
	
	free(leftEdges);
	free(rightEdges);
}

static void initTree(KDAccelTree *kdtree, Primitive* p)
{
	/* int icost, int tcost, float ebonus, int maxp, int md */
	BBox *primBounds;
	int *primNums, *prims0, *prims1;
	int prims_count = kdtree->nPrims;
	int i, j, nEdges;
	BoundEdge *edges;

	for (i = 0; i < prims_count; i++)
	{
		kdtree->primitives[i] = p[i];
	}

	// 지금 트리가 가진 노드의 개수는 0개입니다.
	kdtree->nextFreeNodes = 0;
	kdtree->nAllocednodes = 0;

	// 최대 깊이를 설정함 
	if (kdtree->maxDepth <= 0)
	{
		//TODO: 올바른 계산값으로 고쳐야함
		kdtree->maxDepth = 5;
	}

	// kdtree의 전체 bound 계산 및 각 primitive의 bound를 계산해 놓음
	primBounds = (BBox *)malloc(sizeof(BBox) * prims_count);
	for (i = 0; i < prims_count; i++)
	{
		BBox b = make_bbox_by_triangle(kdtree->primitives[i]);
		kdtree->bounds = union_bbox_and_bbox(kdtree->bounds, b);
		primBounds[i] = b;
	}

	prims0 = (int *)mzalloc(sizeof(int) * prims_count);
	prims1 = (int *)mzalloc(sizeof(int) * prims_count * (kdtree->maxDepth + 1));

	// kdtree 구축을 primNums(노드에 들어있는 프리미티브의 인덱스 모음) 초기화 
	primNums = (int *)malloc(sizeof(int) * prims_count);
	for (i = 0; i < prims_count; i++){
		primNums[i] = i;
	}

	// edges 공간 할당 및 초기화
	edges = (BoundEdge *)malloc(sizeof(BoundEdge) * prims_count * 2 * 3);
	nEdges = 0;
	for (i = 0; i < prims_count; i++)
	{
		int pn = primNums[i];
		BBox bbox = primBounds[pn];

		for (j = 0; j < 3; j++)
		{
			float bbox_min = bbox.faaBounds[0][j], bbox_max = bbox.faaBounds[1][j];

			if (bbox_min == bbox_max)
			{
				init_nlogn_bound_edge(&edges[nEdges++], bbox_min, pn, PLANAR, j);
			}
			else
			{
				init_nlogn_bound_edge(&edges[nEdges++], bbox_min, pn, START, j);
				init_nlogn_bound_edge(&edges[nEdges++], bbox_max, pn, END, j);
			}
		}
	}
	qsort(edges, nEdges, sizeof(BoundEdge), compare_bound);

	// kdtree 구축을 위한 재귀문 실행
	buildTree(kdtree, 0, &kdtree->bounds, kdtree->nPrims, kdtree->maxDepth, edges, nEdges, 0);
	
	// kdtree 구축에 사용한 공간 해제
	free(primBounds);
	free(edges);
	free(prims0);
	free(prims1);
	free(primNums);
}

void nlogn_accel_build(Data *data)
{
	KDAccelTree *kdtree = (KDAccelTree *)mzalloc(sizeof(KDAccelTree));

	if (data->accel_struct) free(data->accel_struct);
	data->accel_struct = (void *)kdtree;

	kdtree->isectCost = 80;
	kdtree->traversalCost = 1;
	kdtree->emptyBonus = 0.5f;
	kdtree->maxPrims = 1;
	kdtree->maxDepth = 10;
	kdtree->nPrims = data->prim_count;

	kdtree->primitives = (Primitive *)malloc(sizeof(data->primitives[0]) * data->prim_count);
	memcpy(kdtree->primitives, data->primitives, sizeof(*kdtree->primitives));
	
	initTree(kdtree, data->primitives);
}
