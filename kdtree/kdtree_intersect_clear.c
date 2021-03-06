﻿
/*
Copyright (c) 2015, Minwoo Lee(hellomw@msl.yonsei.ac.kr)
					Daeyoun Kang(mail.ikasty@gmail.com),
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

#include <string.h>
#include <float.h>
#include "kdtree_intersect_clear.h"

#include "kdtree_type.h"
#include "kdtree_queue.h"
#include "timecheck.h"

static int box_IntersectP(BBox b_box, Ray ray, float *hit_t0, float *hit_t1)
{
	int i;
	float t0 = ray.min_t, t1 = ray.max_t;

	for (i = 0; i < 3; i++)
	{
		// Update interval for _i_th bounding box slab
		float tNear = (b_box.faaBounds[0][i] - ray.orig[i]) / ray.dir[i];
		float tFar  = (b_box.faaBounds[1][i] - ray.orig[i]) / ray.dir[i];

		// Update parametric interval from slab intersection $t$s
		if (tNear > tFar)
		{
			float temp = tNear;
			tNear = tFar;
			tFar = temp;
		}
		t0 = tNear > t0 ? tNear : t0;
		t1 = tFar  < t1 ? tFar  : t1;
		if (t0 > t1) return 0;
	}

	if (hit_t0) *hit_t0 = t0;
	if (hit_t1) *hit_t1 = t1;

	return 1;
}

/**
 * kdtreeTraversal.c.old의 void Intersect 함수에서 가져옴
 */
Hit kdtree_intersect_search(Data *data, Ray *ray)
{
	KDAccelTree *kdtree = (KDAccelTree *)data->accel_struct;
	KDAccelNode *node;
	queue_head(workqueue);
	
	float tmin, tmax;

	USE_TIMECHECK();

	Hit min_hit;
	memset(&min_hit, 0, sizeof(min_hit));
	
	// tmin과 tmax를 계산하는 것과 동시에, 현재 트리와 광선이 교차하는지 검사
	if (!box_IntersectP(kdtree->bounds, *ray, &tmin, &tmax))
	{
		return min_hit;
	}

	// root 노드를 queue에 넣음
	queue_add(&kdtree->nodes[0], workqueue);
	kdtree->nodes[0].min_t = ray->min_t = tmin;
	kdtree->nodes[0].max_t = ray->max_t = tmax;

	// queue 탐색
	while (!is_queue_empty(&workqueue))
	{
		queue_pop(node, workqueue, KDAccelNode);

		tmin = node->min_t;
		tmax = node->max_t;

		if (node->flags == LEAF)
		{
			int i;

			ray->min_t = tmin;
			ray->max_t = tmax;

			for (i = 0; i < node->primitive_count; i++)
			{
				TIMECHECK_START();
				Hit hit = intersect_triangle(ray, node->primitives[i]);
				TIMECHECK_END(intersect_clock);

				if (hit.t > 0 && (hit.t < min_hit.t || min_hit.t == 0)) memcpy(&min_hit, &hit, sizeof(hit));
			}

		}
		else
		{
			KDAccelNode *first_child = &kdtree->nodes[ node->above_child_idx ];
			KDAccelNode *second_child = &kdtree->nodes[ node->below_child_idx ];
			int axis = node->flags;

			int is_below_first =
				(ray->orig[axis] < node->split) || ((ray->orig[axis] == node->split) && (ray->dir[axis] < 0));
			float tplane = (node->split - ray->orig[axis]) / ray->dir[axis];

			if (is_below_first)
			{	// swap first and second child
				first_child = &kdtree->nodes[ node->below_child_idx ];
				second_child = &kdtree->nodes[ node->above_child_idx ];
			}

			// 작업 큐에 넣음
			if (tplane > tmax || tplane <= 0)
			{
				queue_add(first_child, workqueue);
				first_child->min_t = tmin;
				first_child->max_t = tmax;
			}
			else if (tplane < tmin)
			{
				queue_add(second_child, workqueue);
				second_child->min_t = tmin;
				second_child->max_t = tmax;
			}
			else
			{
				first_child->min_t = tmin;
				first_child->max_t = tplane;
				second_child->min_t = tplane;
				second_child->max_t = tmax;

				queue_add(first_child, workqueue);
				queue_add(second_child, workqueue);
			}
		}

	} // while

	return min_hit;
}

void kdtree_clear_accel(Data *data)
{
	int i;
	KDAccelTree *kdtree = (KDAccelTree *)data->accel_struct;

	if (kdtree == NULL) return ;

	free(kdtree->primitives);

	for (i = 0; i < kdtree->nextFreeNodes; i++)
	{
		KDAccelNode *node = (KDAccelNode *)&kdtree->nodes[i];
		if (node->primitives) free(node->primitives);
	}

	free(kdtree->nodes);
	data->accel_struct = zfree(kdtree);
}

