#ifndef KDTREE_TYPE_H
#define KDTREE_TYPE_H

#include "../include/type.h"
#include "kdtree_queue.h"

typedef struct __bbox
{
	float faaBounds[2][3];
} BBox;

typedef struct __acceltree
{
	struct __bbox bounds;
} KDAccelTree;

typedef struct __accelnode
{
	// 하위 node
	struct __accelnode *first_child, *second_child;

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
	
} KDAccelNode;



#endif