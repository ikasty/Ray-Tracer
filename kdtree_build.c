#include "kdtree/kdtree_build.h"
#include "kdtree/bbox.h"
#include "kdtree/boundedge.h"
#include <float.h>
#include <stdlib.h>

int compare(const void* a, const void* b){
	if((*(BoundEdge *)a).t == (*(BoundEdge *)b).t){
		return (*(BoundEdge *)a).e_type < (*(BoundEdge *)b).e_type? -1: 1;
	}
	else{
		return (*(BoundEdge *)a).t - (*(BoundEdge *)b).t;
	}	
};

void initTree(KDAccelTree *kdtree, Primitive* p, int np, int icost, int tcost, 
			  float ebonus, int maxp, int md){
	int i;
	BBox *primBounds;
	int *primNums, *prims0, *prims1;
	BoundEdge *edges[3];

	kdtree->isectCost = icost;
	kdtree->traversalCost = tcost;
	kdtree->maxPrims = maxp;
	kdtree->maxDepth = md;
	kdtree->emptyBonus = ebonus;
	kdtree->nPrims = np;
	kdtree->primitives = (Primitive *)malloc(sizeof(Primitive) * np);
	kdtree->bounds.faaBounds[0][0] = 0;
	kdtree->bounds.faaBounds[0][1] = 0;
	kdtree->bounds.faaBounds[0][2] = 0;
	kdtree->bounds.faaBounds[1][0] = 0;
	kdtree->bounds.faaBounds[1][1] = 0;
	kdtree->bounds.faaBounds[1][2] = 0;

	for(i=0; i<np; i++){
		kdtree->primitives[i] = p[i];
	}

	// 지금 트리가 가진 노드의 개수는 0개입니다.	
	kdtree->nextFreeNodes = 0;
	kdtree->nAllocednodes = 0;
	// 최대 깊이를 설정함 
	if(kdtree->maxDepth <= 0){
		//TODO: 올바른 계산값으로 고쳐야함
		kdtree->maxDepth = 5;
	}

	// kdtree의 전체 bound 계산 및 각 primitive의 bound를 계산해 놓음
	primBounds = (BBox *)malloc(sizeof(BBox)*kdtree->nPrims);
	for(i=0; i<kdtree->nPrims; i++){
		BBox b = make_bbox_by_triangle(kdtree->primitives[i]);
		kdtree->bounds = union_bbox_and_bbox(kdtree->bounds, b);
		primBounds[i] = b;
	}

	// kdtree 구축에 필요한 공간 할당
	for(i=0; i<3; i++){
		edges[i] = (BoundEdge* )malloc(2*sizeof(BoundEdge)*kdtree->nPrims);
	}
	prims0 = (int *)malloc(sizeof(int)*kdtree->nPrims);
	prims1 = (int *)malloc(sizeof(int)*kdtree->nPrims*(kdtree->maxDepth+1));

	// kdtree 구축을 primNums(노드에 들어있는 프리미티브의 인덱스 모음) 초기화 
	primNums = (int *)malloc(sizeof(int)*kdtree->nPrims);
	for(i=0; i<kdtree->nPrims; i++){
		primNums[i] = i;
	}
	// kdtree 구축을 위한 재귀문 실행
	buildTree(kdtree, 0, &kdtree->bounds, primBounds, primNums, kdtree->nPrims,
		kdtree->maxDepth, edges, prims0, prims1, 0);
	
	// kdtree 구축에 사용한 공간 해제
	free(primBounds);
	for(i=0; i<3; i++){
		free(edges[i]);
	}
	free(prims0);
	free(prims1);
	free(primNums);
}

void initLeaf(KDAccelTree* kdtree, KDAccelNode* node, int* primNums, int np){
	int i;
	node->flags = 3;
	node->primitive_count = np;
	node->primitives = (Primitive *)malloc(sizeof(Primitive) * np);
	for(i=0; i<np; i++){
		node->primitives[i] = kdtree->primitives[primNums[i]];
	}
}

void initInterior(KDAccelTree* kdtree, KDAccelNode* node, int axis, int ac, int bc, float s){
	node->split = s;
	node->flags = axis;
	node->above_child = &kdtree->nodes[ac];
	node->below_child = &kdtree->nodes[bc];
}

// build tree 함수는 tree node마다 불러와지므로 
// 인테리어 노드와 리프노드를 구별해야 한다.
void buildTree(KDAccelTree *kdtree, int nodeNum, BBox *nodeBounds, 
			   BBox *allPrimBounds, int *primNums, int nPrimitives,int depth, 
			   BoundEdge *edges[3], int *prims0, int *prims1, int badRefines){
	int i, n0 = 0, n1 = 0;
	int bestAxis = -1, bestOffset = -1, axis;
	int retries = 0;	
	float bestCost = FLT_MAX;
	float oldCost = (float)kdtree->isectCost*nPrimitives;
	float invTotalSA = 1.f / get_surface_of_bbox(nodeBounds);
	float d[3] = {
		nodeBounds->faaBounds[1][0] - nodeBounds->faaBounds[0][0],
		nodeBounds->faaBounds[1][1] - nodeBounds->faaBounds[0][1],
		nodeBounds->faaBounds[1][2] - nodeBounds->faaBounds[0][2]
	};
	int nBelow = 0, nAbove = nPrimitives;
	BBox bounds0, bounds1;

	// 노드 배열로 부터 비어있는 다음 노드를 구함
	// 가득 찼으면 공간 늘려서 새로 할당
	if(kdtree->nextFreeNodes == kdtree->nAllocednodes){
		int nAlloc = 2*kdtree->nAllocednodes>512? 2*kdtree->nAllocednodes:512;
		KDAccelNode* n = (KDAccelNode* )malloc(nAlloc*sizeof(KDAccelNode));
		if(kdtree->nAllocednodes > 0){
			memcpy(n, kdtree->nodes, kdtree->nAllocednodes*sizeof(KDAccelNode));
			free(kdtree->nodes);
		}
		kdtree->nodes = n;
		kdtree->nAllocednodes = nAlloc;
	}
	kdtree->nextFreeNodes++;

	// 종료 조건에 다다르면 리프 노드로 생성
	if(nPrimitives <= kdtree->maxPrims || depth == 0){
		initLeaf(kdtree, &kdtree->nodes[nodeNum], primNums, nPrimitives);
		return;
	}
	// 내부 노드를 초기화 하고 재귀 진행

	// 어느 축으로 자를지 결정함. 가장 길쭉한 축부터 시도
	axis = find_axis_of_maximum_extent(nodeBounds);
	while(retries < 3){
		// 축을 기준으로 edge를 초기화
		for(i=0; i<nPrimitives; i++){
			// 이 노드에 속한 프리미티브를 돌면서 edge를 찾음
			int pn = primNums[i];
			BBox bbox = allPrimBounds[pn];
			init_bound_edge(&edges[axis][2*i], bbox.faaBounds[0][axis], pn, START);
			init_bound_edge(&edges[axis][2*i+1], bbox.faaBounds[1][axis], pn, END);
		}
		qsort(&edges[axis][0], 2*nPrimitives, sizeof(BoundEdge), compare);
		// 베스트를 찾기 위해 모든 코스트를 계산함
		for(i=0; i<2*nPrimitives; i++){
			if(edges[axis][i].e_type == END){
				nAbove--;
			}
			if(edges[axis][i].t > nodeBounds->faaBounds[0][axis] &&
				edges[axis][i].t < nodeBounds->faaBounds[1][axis]){
				// 이 노드 범위 내에 있는 edge를 위한 cost 계산
				int otherAxis0 = (axis + 1)%3, otherAxis1 = (axis + 2)%3;
				float pBelow = 2 * (
					(d[otherAxis0] * d[otherAxis1]) + 
					(edges[axis][i].t - nodeBounds->faaBounds[0][axis]) *
					(d[otherAxis0] + d[otherAxis1])
				) * invTotalSA;
				float pAbove = 2 * (
					(d[otherAxis0] * d[otherAxis1]) + 
					(nodeBounds->faaBounds[1][axis] - edges[axis][i].t) *
					(d[otherAxis0] + d[otherAxis1])
				) * invTotalSA;
				float eb = (nAbove == 0 || nBelow == 0)? kdtree->emptyBonus: 0.0f;
				float cost = kdtree->traversalCost +
					kdtree->isectCost * (1.f - eb) * (pBelow*nBelow + pAbove*nAbove);
				// 최소 코스트라면 갱신
				if(cost < bestCost){
					bestCost = cost;
					bestAxis = axis;
					bestOffset = i;
				}
			}
			if(edges[axis][i].e_type == START){
				//다음 루프를 위한 개수 증가
				nBelow++;
			}
		}
		// 분할할 만한 적당한 위치가 없으면 리프 노드 생성
		if(bestAxis == -1 && retries < 2){
			// 일단 축을 바꿔서 진행해봄
			// (노드가 프리미티브에 비해 너무 작은 경우)
			retries++;
			axis = (axis+1) % 3;
		}
		else{
			if(bestCost>oldCost){
				badRefines++;
			}
			// 결국 축을 못찾았거나, 모든 축의 cost
			if(bestCost > 4.f * oldCost && nPrimitives < 16 ||
				bestAxis == -1 ||
				badRefines == 3){
					initLeaf(kdtree, &kdtree->nodes[nodeNum], primNums, nPrimitives);
					return;
			}
			break;
		}
	}
	// 분할에 대해 프리미티브 분류
	// split 지점 밑에 START가 위치하면 일단 밑에 위치함
	for(i=0; i<bestOffset; i++){
		if(edges[bestAxis][i].e_type == START){
			prims0[n0++] = edges[bestAxis][i].primNum;
		}
	}
	// split 지점 위에 END가 위치하면 일단 위에 위치함
	for(i=bestOffset+1; i<2*nPrimitives; i++){
		if(edges[bestAxis][i].e_type == END){
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

