
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

#ifndef KDTREE_TYPE_H
#define KDTREE_TYPE_H

#include "../include/type.h"
#include "kdtree_queue.h"

typedef struct __bbox BBox;
typedef struct __acceltree KDAccelTree;
typedef struct __accelnode KDAccelNode;

typedef enum edge_type{ START = 3, PLANAR = 2, MID=1, END = 0 } edge_type;
typedef enum side_type{ BOTH = 2, BELOW = 1, ABOVE = 0} side_type;

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