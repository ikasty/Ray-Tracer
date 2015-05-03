#include <stdlib.h>

#include "kdtree_common.h"

#define PLANAR_TRY_TWICE

void init_bound_edge(BoundEdge* bd, float tt, int pn, int t, int a){
	bd->t = tt;
	bd->primNum = pn;
	bd->e_type = t;
	bd->axis = a;
}

void initLeaf(KDAccelTree *kdtree, KDAccelNode *node, int *prim_indexes, int np)
{
	int i;
	node->flags = 3;
	node->primitive_count = np;
	node->primitives = (Primitive *)malloc(sizeof(Primitive) * np);
	for (i = 0; i < np; i++)
	{
		node->primitives[i] = kdtree->primitives[prim_indexes[i]];
	}
}

void initInterior(KDAccelNode *node, KDAccelNode *ac, KDAccelNode *bc, int axis, float s)
{
	node->split = s;
	node->flags = axis;
	node->above_child = ac;
	node->below_child = bc;
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

#ifdef	PLANAR_TRY_TWICE
	float above_cost;
#endif

	// planar를 below에 두고 코스트 계산
	nBelow += nPlanar;

	eb = (nBelow == 0 || nAbove == 0) ? kdtree->emptyBonus : 0.0f;
	cost = kdtree->traversalCost;
	cost += kdtree->isectCost * (1.f - eb) * (pBelow * nBelow + pAbove * nAbove);

	nBelow -= nPlanar;

	*side = BELOW;


#ifdef	PLANAR_TRY_TWICE
	// planar를 above에 두고 코스트 계산
	nAbove += nPlanar;

	eb = (nBelow == 0 || nAbove == 0) ? kdtree->emptyBonus : 0.0f;
	above_cost = kdtree->traversalCost;
	above_cost += kdtree->isectCost * (1.f - eb) * (pBelow * nBelow + pAbove * nAbove);

	nAbove -= nPlanar;

	if (cost > above_cost)
	{
		cost = above_cost;
		*side = ABOVE;
	}
#endif

	return cost;
}