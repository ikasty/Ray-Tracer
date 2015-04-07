#include <float.h>
#include <stdlib.h>
#include "kdtree_build.h"

#include "kdtree_type.h"
#include "bbox.h"
#include "boundedge.h"
#include "../include/debug-msg.h"

static void initLeaf(KDAccelTree* kdtree, KDAccelNode* node, int* primNums, int np);
static void initInterior(KDAccelTree* kdtree, KDAccelNode* node, int axis, int ac, int bc, float s);
static void buildTree(KDAccelTree *kdtree, int nodeNum, BBox *nodeBounds, 
			   BBox *allPrimBounds, int *primNums, int nPrimitives,int depth, 
			   BoundEdge *edges[3], int *prims0, int *prims1, int badRefines);
static void initTree(KDAccelTree *kdtree, Primitive* p);

static int compare_bound(const void *a, const void *b)
{
	BoundEdge *l = (BoundEdge *)a, *r = (BoundEdge *)b;

	if (l->t == r->t)
		return (l->e_type) > (r->e_type)? 1: -1;
	else
		return (l->t) > (r->t)? 1: -1;
};

static void initLeaf(KDAccelTree *kdtree, KDAccelNode *node, int *primNums, int np)
{
	int i;
	node->flags = 3;
	node->primitive_count = np;
	node->primitives = (Primitive *)malloc(sizeof(Primitive) * np);
	for (i = 0; i < np; i++)
	{
		node->primitives[i] = kdtree->primitives[primNums[i]];
	}
}

void initInterior(KDAccelTree *kdtree, KDAccelNode *node, int axis, int ac, int bc, float s)
{
	node->split = s;
	node->flags = axis;
	node->above_child = &kdtree->nodes[ac];
	node->below_child = &kdtree->nodes[bc];
}

// build tree 함수는 tree node마다 불러와지므로 
// 인테리어 노드와 리프노드를 구별해야 한다.
void buildTree(KDAccelTree *kdtree, int nodeNum, BBox *nodeBounds,
			   BBox *allPrimBounds, int *primNums, int nPrimitives, int depth,
			   BoundEdge *edges[3], int *prims0, int *prims1, int badRefines)
{
	int i, j, n0 = 0, n1 = 0, nEdges;
	int bestAxis = -1, bestOffset = -1, axis;
	int retries = 0;
	float bestCost = FLT_MAX;
	float oldCost = (float)kdtree->isectCost * nPrimitives;
	float invTotalSA = 1.f / get_surface_of_bbox(nodeBounds);
	float d[3] = {
		nodeBounds->faaBounds[1][0] - nodeBounds->faaBounds[0][0],
		nodeBounds->faaBounds[1][1] - nodeBounds->faaBounds[0][1],
		nodeBounds->faaBounds[1][2] - nodeBounds->faaBounds[0][2]
	};
	BBox bounds0, bounds1;

	int nBelow[3], nPlanar[3], nAbove[3];
	BoundEdge bestPlane;
	int** locationsOfPrims;


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
		initLeaf(kdtree, &kdtree->nodes[nodeNum], primNums, nPrimitives);
		return;
	}
	// 내부 노드를 초기화 하고 재귀 진행


	// 모든 축의 edge를 edges하나에 저장함 
	nEdges = 0;
	for (i=0; i<nPrimitives; i++)
	{
		// 이 노드에 속한 프리미티브를 돌면서 edge를 찾음
		int pn = primNums[i];
		BBox bbox = allPrimBounds[pn];

		for (j=0; j<3; j++)
		{
			float bbox_min = bbox.faaBounds[0][j], bbox_max = bbox.faaBounds[1][j];

			if (bbox_min == bbox_max)
			{
				init_bound_edge(&edges[nEdges++], bbox_min, pn, PLANAR, j);
			}
			else
			{
				init_bound_edge(&edges[nEdges++], bbox_max, pn, START, j);
				init_bound_edge(&edges[nEdges++], bbox_max, pn, END, j);
			}
		}
	}
	qsort(edges, nEdges, sizeof(BoundEdge), compare_bound);

	{
		// bestPlane 초기화
		bestPlane.axis = -1;

		// 평면으로 나누어진 영역에 속한 prim 개수 초기화 
		for(i=0; i<3; i++)
		{
			nAbove[i] = nPrimitives;
			nPlanar[i] = 0;
			nBelow[i] = 0;
		}

		// edge로 평면을 만들어 나눴을 때 cost가 최소가 되는 평면 구함
		i = 0;
		while(i<nEdges)
		{
			BoundEdge curPlane = edges[i];
			int nEndOfCurPlane=0, nPlanarOfCurPlane=0, nStartOfCurPlane=0;
			float belowPlanarCost, abovePlanarCost;
			float minBound = nodeBounds->faaBounds[0][curPlane.axis];
			float maxBound = nodeBounds->faaBounds[1][curPlane.axis];

			// 현재 평면에 닿아 있는 edge들을 분류함 
			while(i<nEdges && edges[i].axis==curPlane.axis && edges[i].t==curPlane.t)
			{
				switch(edges[i++].e_type){
				case END:			nEndOfCurPlane++;			break;
				case PLANAR: 	nPlanarOfCurPlane++; 	break;
				case START: 	nStartOfCurPlane++; 	break;
				}
			}
			nAbove[curPlane.axis] -= (nEndOfCurPlane + nPlanarOfCurPlane);
			nPlanar[curPlane.axis] = nPlanarOfCurPlane;
			
			// 코스트 계산 
			if ( minBound < curPlane.t && curPlane.t < maxBound)
			{
				// 이 노드 범위 내에 있는 edge를 위한 cost 계산
				int nModiAbove, nModiBelow;
				float eb, belowPlanarCost, abovePlanarCost;
				int otherAxis0 = (curPlane.axis + 1) % 3, otherAxis1 = (curPlane.axis + 2) % 3;
				float pBelow = 2 * (
					(d[otherAxis0] * d[otherAxis1]) + (curPlane.t - minBound) *	(d[otherAxis0] + d[otherAxis1])
				) * invTotalSA;
				float pAbove = 2 * (
					(d[otherAxis0] * d[otherAxis1]) + (maxBound - curPlane.t) *	(d[otherAxis0] + d[otherAxis1])
				) * invTotalSA;

				// planar를 below에 두고 코스트 계산 
				nModiBelow = nBelow[curPlane.axis] + nPlanar[curPlane.axis];
				nModiAbove = nAbove[curPlane.axis];
				eb = (nModiAbove == 0 || nModiBelow == 0) ? kdtree->emptyBonus : 0.0f;
				belowPlanarCost = kdtree->traversalCost +
					kdtree->isectCost * (1.f - eb) * (pBelow * nModiBelow + pAbove * nModiAbove);

				// planar를 above에 두고 코스트 계산 
				nModiBelow = nBelow[curPlane.axis];
				nModiAbove = nAbove[curPlane.axis] + nPlanar[curPlane.axis];
				eb = (nModiAbove == 0 || nModiBelow == 0) ? kdtree->emptyBonus : 0.0f;
				abovePlanarCost = kdtree->traversalCost +
					kdtree->isectCost * (1.f - eb) * (pBelow * nModiBelow + pAbove * nModiAbove);
			}
			
			// 현재 코스트와 최소 코스트의 비교
			if(belowPlanarCost<bestCost || abovePlanarCost<bestCost)
			{
				bestCost = cost;
				bestPlane = curPlane;
				bestSide = belowPlanarCost<abovePlanarCost? BELOW: ABOVE;
			}

			// 다음 loop를 위한 변수 조정 
			nPlanar[curPlane.axis] = 0;
			nBelow[curPlane.axis] += (nPlanarOfCurPlane + nStartOfCurPlane);
		}// while
	}
	if (bestCost > oldCost) badRefines++;
	// 분할할 만한 적당한 위치가 없으면 리프 노드 생성
	if ((bestCost > 4.f * oldCost && nPrimitives < 16) || bestPlane.axis == -1 ||	badRefines >= 3)
	{
		initLeaf(kdtree, &kdtree->nodes[nodeNum], primNums, nPrimitives);
		return;
	}

	{

		for(i=0; i<nPrimitives; i++)
		{

		}
	}
	







		

			

	}
	// 분할에 대해 프리미티브 분류
	// split 지점 밑에 START가 위치하면 일단 밑에 위치함
	for (i = 0; i <= bestOffset; i++)
	{
		if (edges[bestAxis][i].e_type == START)
		{
			prims0[n0++] = edges[bestAxis][i].primNum;
		}
	}
	// split 지점 위에 END가 위치하면 일단 위에 위치함
	for (i = bestOffset; i < 2 * nPrimitives; i++)
	{
		if (edges[bestAxis][i].e_type == END)
		{
			prims1[n1++] = edges[bestAxis][i].primNum;
		}
	}
	// 재귀적으로 자식 노드 초기화
	bounds0 = *nodeBounds;
	bounds1 = *nodeBounds;
	bounds0.faaBounds[0][bestAxis] = edges[bestAxis][bestOffset].t;
	bounds1.faaBounds[1][bestAxis] = edges[bestAxis][bestOffset].t;
	buildTree(kdtree, nodeNum+1, &bounds0,
		allPrimBounds, prims0, n0, depth-1, edges,
		prims0, prims1+nPrimitives, badRefines);
	initInterior(kdtree, &kdtree->nodes[nodeNum], bestAxis, 
		kdtree->nextFreeNodes, nodeNum+1, edges[bestAxis][bestOffset].t);
	buildTree(kdtree, kdtree->nextFreeNodes, &bounds1,
		allPrimBounds, prims1, n1, depth -1, edges,
		prims0, prims1+nPrimitives, badRefines);
}

static void initTree(KDAccelTree *kdtree, Primitive* p)
{
	/* int icost, int tcost, float ebonus, int maxp, int md */
	BBox *primBounds;
	int *primNums, *prims0, *prims1;
	int prims_count = kdtree->nPrims;
	BoundEdge *edges[3];

	int i;

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

	// kdtree 구축에 필요한 공간 할당
	for (i = 0; i < 3; i++)
	{
		edges[i] = (BoundEdge *)malloc(sizeof(BoundEdge) * prims_count * 2);
	}
	prims0 = (int *)mzalloc(sizeof(int) * prims_count);
	prims1 = (int *)mzalloc(sizeof(int) * prims_count * (kdtree->maxDepth + 1));

	// kdtree 구축을 primNums(노드에 들어있는 프리미티브의 인덱스 모음) 초기화 
	primNums = (int *)malloc(sizeof(int) * prims_count);
	for (i = 0; i < prims_count; i++)
	{
		primNums[i] = i;
	}
	
	// kdtree 구축을 위한 재귀문 실행
	buildTree(kdtree, 0, &kdtree->bounds, primBounds, primNums, kdtree->nPrims,
		kdtree->maxDepth, edges, prims0, prims1, 0);
	
	// kdtree 구축에 사용한 공간 해제
	free(primBounds);
	for (i = 0; i < 3; i++)
	{
		free(edges[i]);
	}
	free(prims0);
	free(prims1);
	free(primNums);
}

void kdtree_accel_build(Data *data)
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
