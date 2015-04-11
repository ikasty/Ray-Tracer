
/*
Copyright (c) 2015, Daeyoun Kang(mail.ikasty@gmail.com),
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

#include <stdlib.h>

#include "kdtree_common.h"

#define PLANAR_TRY_TWICE

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
	cost = (float)kdtree->traversalCost;
	cost += kdtree->isectCost * (1.f - eb) * (pBelow * nBelow + pAbove * nAbove);

	nBelow -= nPlanar;

	*side = BELOW;


#ifdef	PLANAR_TRY_TWICE
	// planar를 above에 두고 코스트 계산
	nAbove += nPlanar;

	eb = (nBelow == 0 || nAbove == 0) ? kdtree->emptyBonus : 0.0f;
	above_cost = (float)kdtree->traversalCost;
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