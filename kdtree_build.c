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

	// ���� Ʈ���� ���� ����� ������ 0���Դϴ�.	
	kdtree->nextFreeNodes = 0;
	kdtree->nAllocednodes = 0;
	// �ִ� ���̸� ������ 
	if(kdtree->maxDepth <= 0){
		//TODO: �ùٸ� ��갪���� ���ľ���
		kdtree->maxDepth = 5;
	}

	// kdtree�� ��ü bound ��� �� �� primitive�� bound�� ����� ����
	primBounds = (BBox *)malloc(sizeof(BBox)*kdtree->nPrims);
	for(i=0; i<kdtree->nPrims; i++){
		BBox b = make_bbox_by_triangle(kdtree->primitives[i]);
		kdtree->bounds = union_bbox_and_bbox(kdtree->bounds, b);
		primBounds[i] = b;
	}

	// kdtree ���࿡ �ʿ��� ���� �Ҵ�
	for(i=0; i<3; i++){
		edges[i] = (BoundEdge* )malloc(2*sizeof(BoundEdge)*kdtree->nPrims);
	}
	prims0 = (int *)malloc(sizeof(int)*kdtree->nPrims);
	prims1 = (int *)malloc(sizeof(int)*kdtree->nPrims*(kdtree->maxDepth+1));

	// kdtree ������ primNums(��忡 ����ִ� ������Ƽ���� �ε��� ����) �ʱ�ȭ 
	primNums = (int *)malloc(sizeof(int)*kdtree->nPrims);
	for(i=0; i<kdtree->nPrims; i++){
		primNums[i] = i;
	}
	// kdtree ������ ���� ��͹� ����
	buildTree(kdtree, 0, &kdtree->bounds, primBounds, primNums, kdtree->nPrims,
		kdtree->maxDepth, edges, prims0, prims1, 0);
	
	// kdtree ���࿡ ����� ���� ����
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

// build tree �Լ��� tree node���� �ҷ������Ƿ� 
// ���׸��� ���� ������带 �����ؾ� �Ѵ�.
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

	// ��� �迭�� ���� ����ִ� ���� ��带 ����
	// ���� á���� ���� �÷��� ���� �Ҵ�
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

	// ���� ���ǿ� �ٴٸ��� ���� ���� ����
	if(nPrimitives <= kdtree->maxPrims || depth == 0){
		initLeaf(kdtree, &kdtree->nodes[nodeNum], primNums, nPrimitives);
		return;
	}
	// ���� ��带 �ʱ�ȭ �ϰ� ��� ����

	// ��� ������ �ڸ��� ������. ���� ������ ����� �õ�
	axis = find_axis_of_maximum_extent(nodeBounds);
	while(retries < 3){
		// ���� �������� edge�� �ʱ�ȭ
		for(i=0; i<nPrimitives; i++){
			// �� ��忡 ���� ������Ƽ�긦 ���鼭 edge�� ã��
			int pn = primNums[i];
			BBox bbox = allPrimBounds[pn];
			init_bound_edge(&edges[axis][2*i], bbox.faaBounds[0][axis], pn, START);
			init_bound_edge(&edges[axis][2*i+1], bbox.faaBounds[1][axis], pn, END);
		}
		qsort(&edges[axis][0], 2*nPrimitives, sizeof(BoundEdge), compare);
		// ����Ʈ�� ã�� ���� ��� �ڽ�Ʈ�� �����
		for(i=0; i<2*nPrimitives; i++){
			if(edges[axis][i].e_type == END){
				nAbove--;
			}
			if(edges[axis][i].t > nodeBounds->faaBounds[0][axis] &&
				edges[axis][i].t < nodeBounds->faaBounds[1][axis]){
				// �� ��� ���� ���� �ִ� edge�� ���� cost ���
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
				// �ּ� �ڽ�Ʈ��� ����
				if(cost < bestCost){
					bestCost = cost;
					bestAxis = axis;
					bestOffset = i;
				}
			}
			if(edges[axis][i].e_type == START){
				//���� ������ ���� ���� ����
				nBelow++;
			}
		}
		// ������ ���� ������ ��ġ�� ������ ���� ��� ����
		if(bestAxis == -1 && retries < 2){
			// �ϴ� ���� �ٲ㼭 �����غ�
			// (��尡 ������Ƽ�꿡 ���� �ʹ� ���� ���)
			retries++;
			axis = (axis+1) % 3;
		}
		else{
			if(bestCost>oldCost){
				badRefines++;
			}
			// �ᱹ ���� ��ã�Ұų�, ��� ���� cost
			if(bestCost > 4.f * oldCost && nPrimitives < 16 ||
				bestAxis == -1 ||
				badRefines == 3){
					initLeaf(kdtree, &kdtree->nodes[nodeNum], primNums, nPrimitives);
					return;
			}
			break;
		}
	}
	// ���ҿ� ���� ������Ƽ�� �з�
	// split ���� �ؿ� START�� ��ġ�ϸ� �ϴ� �ؿ� ��ġ��
	for(i=0; i<bestOffset; i++){
		if(edges[bestAxis][i].e_type == START){
			prims0[n0++] = edges[bestAxis][i].primNum;
		}
	}
	// split ���� ���� END�� ��ġ�ϸ� �ϴ� ���� ��ġ��
	for(i=bestOffset+1; i<2*nPrimitives; i++){
		if(edges[bestAxis][i].e_type == END){
			prims1[n1++] = edges[bestAxis][i].primNum;
		}
	}
	// ��������� �ڽ� ��� �ʱ�ȭ
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

