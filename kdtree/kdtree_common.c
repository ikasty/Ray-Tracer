#include <stdlib.h>

#include "kdtree_common.h"

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