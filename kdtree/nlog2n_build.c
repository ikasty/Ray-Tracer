#include <float.h>
#include <stdlib.h>
#include "nlog2n_build.h"

#include "kdtree_type.h"
#include "bbox.h"
#include "boundedge.h"
#include "../include/debug-msg.h"

static int compare_bound(const void *a, const void *b)
{
	BoundEdge *l = (BoundEdge *)a, *r = (BoundEdge *)b;

	if (l->t == r->t)
		return (l->e_type) < (r->e_type)? 1: -1;
	else
		return (l->t) > (r->t)? 1: -1;
};

static void initLeaf(KDAccelTree *kdtree, KDAccelNode *node, int *prim_indexes, int np)
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

static void initInterior(KDAccelNode *node, KDAccelNode *ac, KDAccelNode *bc, int axis, float s)
{
	node->split = s;
	node->flags = axis;
	node->above_child = ac;
	node->below_child = bc;
}

static void allocChild(KDAccelTree *kdtree)
{
	// 노드 배열로 부터 비어있는 다음 노드를 구함
	// 가득 찼으면 공간 늘려서 새로 할당
	if (kdtree->nextFreeNodes == kdtree->nAllocednodes)
	{
		kdtree->nAllocednodes *= 2;
		kdtree->nodes = (KDAccelNode *)realloc(kdtree->nodes, sizeof(KDAccelNode) * kdtree->nAllocednodes);
	}
	kdtree->nextFreeNodes++;
}

// build tree 함수는 tree node마다 불러와지므로 
// 인테리어 노드와 리프노드를 구별해야 한다.
static void buildTree(KDAccelTree *kdtree, KDAccelNode *current_node, BBox *nodeBounds,
			BBox *allPrimBounds, int *prim_indexes, int total_prim_counts, int depth,
			BoundEdge *edge_buffer[3], int *below_prims, int *above_prims, int badRefines)
{
	KDAccelNode *above_child, *below_child;
	int nCount = 0, below_count = 0, above_count = 0;
	int bestAxis = -1, axis, bestSide;
	float bestSplit = -1.0f;
	int retries = 0;
	float bestCost = FLT_MAX;
	float oldCost = (float)kdtree->isectCost * total_prim_counts;
	float invTotalSA = 1.f / get_surface_of_bbox(nodeBounds);
	float d[3] = {
		nodeBounds->faaBounds[1][0] - nodeBounds->faaBounds[0][0],
		nodeBounds->faaBounds[1][1] - nodeBounds->faaBounds[0][1],
		nodeBounds->faaBounds[1][2] - nodeBounds->faaBounds[0][2]
	};
	BBox bounds0, bounds1;

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
				init_bound_edge(&edge_buffer[axis][nCount++], bbox.faaBounds[0][axis], pn, PLANAR);
			}
			else
			{
				init_bound_edge(&edge_buffer[axis][nCount++], bbox.faaBounds[0][axis], pn, START);
				init_bound_edge(&edge_buffer[axis][nCount++], bbox.faaBounds[1][axis], pn, END);
			}
		}
		qsort(edge_buffer[axis], nCount, sizeof(BoundEdge), compare_bound);
		
		// 베스트를 찾기 위해 모든 코스트를 계산함
		i = 0;
		while (i < nCount)
		{
			BoundEdge split_edge = edge_buffer[axis][i];
			int start_count = 0, end_count = 0, planar_count = 0;
			float belowPlanarCost, abovePlanarCost, eb;

			// 현재 split 후보에 걸치고 있는 primitive의 타입을 체크한다
			while (	(i < nCount) && (split_edge.t == edge_buffer[axis][i].t) )
			{
				if (edge_buffer[axis][i].e_type == START)	start_count++;
				if (edge_buffer[axis][i].e_type == END)		end_count++;
				if (edge_buffer[axis][i].e_type == PLANAR)	planar_count++;
				i++;
			}

			// split 후보 위에 있는 primitive 개수 갱신
			nAbove -= (end_count + planar_count);
			nPlanar = planar_count;

			if (split_edge.t > nodeBounds->faaBounds[0][axis] &&
			    split_edge.t < nodeBounds->faaBounds[1][axis])
			{
				// 이 노드 범위 내에 있는 edge를 위한 cost 계산
				int otherAxis0 = (axis + 1) % 3, otherAxis1 = (axis + 2) % 3;
				float pBelow = 2 * (
					(d[otherAxis0] * d[otherAxis1]) + 
					(split_edge.t - nodeBounds->faaBounds[0][axis]) *
					(d[otherAxis0] + d[otherAxis1])
				) * invTotalSA;
				float pAbove = 2 * (
					(d[otherAxis0] * d[otherAxis1]) + 
					(nodeBounds->faaBounds[1][axis] - split_edge.t) *
					(d[otherAxis0] + d[otherAxis1])
				) * invTotalSA;

				// planar를 below에 두고 코스트 계산 
				eb = (nBelow+nPlanar == 0 || nAbove == 0) ? kdtree->emptyBonus : 0.0f;
				belowPlanarCost = kdtree->traversalCost +
					kdtree->isectCost * (1.f - eb) * (pBelow * (nBelow+nPlanar) + pAbove * nAbove);

				// planar를 above에 두고 코스트 계산 
				eb = (nBelow == 0 || nAbove+nPlanar == 0) ? kdtree->emptyBonus : 0.0f;
				abovePlanarCost = kdtree->traversalCost +
					kdtree->isectCost * (1.f - eb) * (pBelow * nBelow + pAbove * (nAbove+nPlanar));

				// 최소 코스트라면 갱신
				if(belowPlanarCost<bestCost || abovePlanarCost<bestCost)
				{					
					bestAxis = axis;
					bestSplit = split_edge.t;
					if(belowPlanarCost<abovePlanarCost)
					{
						bestCost = belowPlanarCost;
						bestSide = LEFT;
					}
					else
					{
						bestCost = abovePlanarCost;
						bestSide = RIGHT;
					}
					DEBUG_ONLY(
						if (bestCost < 0) 
							PDEBUG("nBelow %d, pBelow %.2f, nAbove %d, pAbove %.2f\n", nBelow, pBelow, nAbove, pAbove);
					);
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

	// 분할에 대해 프리미티브 분류
	// split 지점 밑에 START가 위치하면 일단 밑에 위치함
	for (i = 0; i < nCount && edge_buffer[bestAxis][i].t < bestSplit; i++)
	{
		if (edge_buffer[bestAxis][i].e_type == START || edge_buffer[bestAxis][i].e_type == PLANAR)
		{
			below_prims[below_count++] = edge_buffer[bestAxis][i].primNum;
		}
	}
	
	// 자르는 평면에 맞닿아 있는 edge들 처리
	for (; i < nCount && edge_buffer[bestAxis][i].t == bestSplit; i++)
	{
		// planar면 bestSide에 따라 결정
		if (edge_buffer[bestAxis][i].e_type == PLANAR){
			// 우리는 planar를 아래에 두기로 결정했었음
			if(bestSide == LEFT)
				below_prims[below_count++] = edge_buffer[bestAxis][i].primNum;
			// 우리는 planar를 위에 두기로 결정했었음
			if(bestSide == RIGHT)
				above_prims[above_count++] = edge_buffer[bestAxis][i].primNum;
		}
	}
	// split 지점 위에 END가 위치하면 일단 위에 위치함
	for (; i < nCount && edge_buffer[bestAxis][i].t > bestSplit; i++)
	{
		if (edge_buffer[bestAxis][i].e_type == END || edge_buffer[bestAxis][i].e_type == PLANAR)
		{
			above_prims[above_count++] = edge_buffer[bestAxis][i].primNum;
		}
	}

	PDEBUG("nlog2n buildTree depth %d, cost %f, below %d, above %d\n", 10 - depth, bestCost, below_count, above_count);

	// 재귀적으로 자식 노드 초기화
	bounds0 = *nodeBounds;
	bounds1 = *nodeBounds;
	bounds0.faaBounds[0][bestAxis] = bestSplit;
	bounds1.faaBounds[1][bestAxis] = bestSplit;

	// 아래 노드 탐색
	below_child = &kdtree->nodes[ kdtree->nextFreeNodes ];
	buildTree(kdtree, below_child, &bounds0,
		allPrimBounds, below_prims, below_count, depth - 1, edge_buffer,
		below_prims, above_prims + total_prim_counts, badRefines);

	// 위 노드 탐색
	above_child = &kdtree->nodes[ kdtree->nextFreeNodes ];
	buildTree(kdtree, above_child, &bounds1,
		allPrimBounds, above_prims, above_count, depth - 1, edge_buffer,
		below_prims, above_prims + total_prim_counts, badRefines);

	// 현재 노드를 부모 노드로 설정
	initInterior(current_node, above_child, below_child, bestAxis, bestSplit);
}

static void initTree(KDAccelTree *kdtree, Primitive* p)
{
	/* int icost, int tcost, float ebonus, int maxp, int md */
	BBox *primBounds;
	int *prim_indexes, *prims0, *prims1;
	int prims_count = kdtree->nPrims;
	BoundEdge *edge_buffer[3];

	int i;

	for (i = 0; i < prims_count; i++)
	{
		kdtree->primitives[i] = p[i];
	}

	// 지금 트리가 가진 노드의 개수는 0개입니다.	
	kdtree->nextFreeNodes = 0;
	kdtree->nAllocednodes = 256;
	kdtree->nodes = (KDAccelNode *)mzalloc(sizeof(KDAccelNode) * 512);

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
		edge_buffer[i] = (BoundEdge *)malloc(sizeof(BoundEdge) * prims_count * 2);
	}
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
		kdtree->maxDepth, edge_buffer, prims0, prims1, 0);
	
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
