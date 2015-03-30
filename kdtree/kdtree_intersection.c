#include "kdtree_intersection.h"

#include "kdtree_type.h"
#include "kdtree_queue.h"

/**
 * kdtreeTraversal.c.old의 void Intersect 함수에서 가져옴
 */
Hit kdtree_intersect_search(Data *data, Ray *ray)
{
	KDAccelTree *accel_tree = (KDAccelTree *)data->accel_struct;
	KDAccelNode *node;
	queue_head(workqueue);
	
	float tmin, tmax;

	Hit min_hit;
	memset(&min_hit, 0, sizeof(min_hit));

	// tmin과 tmax를 계산하는 것과 동시에, 현재 트리와 광선이 교차하는지 검사
	if (!box_IntersectP(accel_tree->bounds, ray, &tmin, &tmax))
	{
		return min_hit;
	}

	// root 노드를 queue에 넣음
	queue_add(accel_tree[0], workqueue);

	// queue 탐색
	while (!is_queue_empty(workqueue))
	{
		queue_pop(node, workqueue, KDAccelNode);

		if (node->flags == LEAF)
		{
			int i;
			for (i = 0; i < node->primitive_count; i++)
			{
				Hit hit = intersect_triangle(ray, node->primitives[i]);
				if (hit.t > 0) memcpy(&min_hit, &hit, sizeof(hit));
			}

		}
		else
		{
			KDAccelNode *first_child = node->above_child;
			KDAccelNode *second_child = node->below_child;
			int axis = node->flags;

			int is_below_first =
				(ray->orig[axis] < node->split) || ((ray->orig[axis] == node->split) && (ray->dir[axis] < 0));
			float tplane = (node->split - ray.orig[axis]) / ray.dir[axis];

			if (is_below_first)
			{	// swap first and second child
				first_child = node->below_child;
				second_child = node->above_child;
			}

			// 작업 큐에 넣음
			if (tplane > tmax || tplane <= 0)
			{
				queue_add(first_child, workqueue);
			}
			else if (tplane < tmin)
			{
				queue_add(second_child, workqueue);
			}
			else
			{
				queue_add(first_child, workqueue);
				queue_add(second_child, workqueue);
			}
		}

	} // while

	return min_hit;
}

static bool box_IntersectP(BBox b_box, Ray ray, float *hit_t0, float *hit_t1)
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
		if (t0 > t1) return false;
	}

	if (hit_t0) *hit_t0 = t0;
	if (hit_t1) *hit_t1 = t1;

	return true;
}