#include <stdlib.h>

#include "kdtree_common.h"
#include "include/debug-msg.h"

DEBUG_ONLY(
int leaf_count = 0;
int leaf_prim_count = 0;
int leaf_max_prim_count = 0;

void leaf_info_print()
{
	PDEBUG("kdtree leaf info: %d leaf created, total %d prims, max %d prims exists\n", leaf_count, leaf_prim_count, leaf_max_prim_count);
}

);

void init_bound_edge(BoundEdge* boundedge, float split_t, int prim_num, int type, int axis){
	boundedge->t = split_t;
	boundedge->primNum = prim_num;
	boundedge->e_type = type;
	boundedge->axis = axis;
}

void initLeaf(KDAccelTree *kdtree, int node_idx, int *prim_indexes, int np)
{
	int i;
	KDAccelNode *node = &kdtree->nodes[node_idx];

	node->flags = 3;
	node->primitive_count = np;
	node->primitives = (Primitive *)malloc(sizeof(Primitive) * np);
	for (i = 0; i < np; i++)
	{
		node->primitives[i] = kdtree->primitives[prim_indexes[i]];
	}

	DEBUG_ONLY(
		leaf_count++;
		leaf_prim_count += np;
		if (leaf_max_prim_count < np) leaf_max_prim_count = np;
	);
}

void initInterior(KDAccelTree *kdtree, int node_idx, int below_child_idx, int above_child_idx, int axis, float s)
{
	KDAccelNode *node = &kdtree->nodes[node_idx];

	node->split = s;
	node->flags = axis;
	node->above_child_idx = above_child_idx;
	node->below_child_idx = below_child_idx;
	node->primitives = NULL;
}

void allocChild(KDAccelTree *kdtree)
{
	// 노드 배열로부터 비어있는 다음 노드를 구함
	// 가득 찼으면 공간 늘려서 새로 할당
	if (kdtree->nextFreeNodes == kdtree->nAllocednodes)
	{
		kdtree->nAllocednodes *= 2;
		kdtree->nodes = (KDAccelNode *)realloc(kdtree->nodes, sizeof(KDAccelNode) * kdtree->nAllocednodes);
	}
	kdtree->nextFreeNodes++;
}

float getCost(KDAccelTree *kdtree, int nBelow, int nPlanar, int nAbove, float pBelow, float pAbove, int *side)
{
	float cost, eb;
	float above_cost;

	// planar를 below에 두고 코스트 계산
	nBelow += nPlanar;

	eb = (nBelow == 0 || nAbove == 0) ? kdtree->emptyBonus : 0.0f;
	cost = (float)kdtree->traversalCost;
	cost += kdtree->isectCost * (1.f - eb) * (pBelow * nBelow + pAbove * nAbove);

	nBelow -= nPlanar;
	*side = BELOW;

	// 플래너 개수가 0이라면 빠른 리턴
	if (nPlanar == 0) return cost;

	// planar를 above에 두고 코스트 계산
	nAbove += nPlanar;

	eb = (nBelow == 0 || nAbove == 0) ? kdtree->emptyBonus : 0.0f;
	above_cost = (float)kdtree->traversalCost;
	above_cost += kdtree->isectCost * (1.f - eb) * (pBelow * nBelow + pAbove * nAbove);

	// above에 두고 계산한 값이 더 작다면 대체
	if (cost > above_cost)
	{
		cost = above_cost;
		*side = ABOVE;
	}

	return cost;
}