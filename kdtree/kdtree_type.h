﻿#ifndef KDTREE_TYPE_H
#define KDTREE_TYPE_H

#include "../include/type.h"
#include "kdtree_queue.h"

typedef struct __bbox BBox;
typedef struct __acceltree KDAccelTree;
typedef struct __accelnode KDAccelNode;

typedef enum edge_type{ START = 3, PLANAR = 2, MID=1, END = 0 } edge_type;
typedef enum side_type{ LEFT = 1, RIGHT = 0} side_type;

struct __bbox
{
	float faaBounds[2][3];
};

struct __accelnode
{
	// 이 node의 상태 - x,y,z축으로 나뉘어 졌는지 또는 leaf node인지
	int flags;

	// 나누는 node라면 split 되는 지점
	float split;

	// intersection search용 queue
	queue_item(workqueue);

	// 이 노드가 포함하고 있는 primitive에 관한 정보
	int primitive_count;
	Primitive *primitives;

	KDAccelNode *above_child;
	KDAccelNode *below_child;
};

struct __acceltree
{
	KDAccelNode* nodes;
	int nAllocednodes, nextFreeNodes;
	//한 노드에 들어갈 수 있는 프리미티브의 최소 개수
	int maxPrims;

	struct __bbox bounds;
	
	Primitive* primitives;
	int nPrims;

	int isectCost, traversalCost, maxDepth;
	float emptyBonus;
};

// 공간을 나누는 평면에 대한 정보를 담고 있다.
// cost를 계산할 candidate가 된다.
typedef struct{
	// 삼각형의 start인지 end인지 저장 
	int e_type;
	
	// 이 평면에 다다르기까지의 t
	float t;
	// ???
	float other_t;
	// 소속 prim num 개수
	int primNum;
	
	// 축 정보
	int axis;
} BoundEdge;

#endif