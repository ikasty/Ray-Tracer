#include <float.h>
#include <stdlib.h>
#include "nlogn_build.h"
#include "kdtree_common.h"

#include "kdtree_type.h"
#include "bbox.h"

#include "include/debug-msg.h"

#define PERCENT 0.2

static int compare_bound(const void *a, const void *b)
{
	BoundEdge *l = (BoundEdge *)a, *r = (BoundEdge *)b;

	// 1. t������ ����
	if (l->t != r->t)
		return (l->t) > (r->t) ? 1 : -1;

	// 2. axis�� ����
	if (l->axis != r->axis)
		return (l->axis) > (r->axis) ? 1 : -1;

	// 3. type�� ����
	return (l->e_type) > (r->e_type) ? 1 : -1;
}

static void merge_bound(BoundEdge* result, BoundEdge* e1, BoundEdge* e2, int nE1, int nE2)
{
	int rCount = 0, i = 0, j = 0;

	while (i < nE1 || j < nE2)
	{
		if (i < nE1 && j < nE2)
		{
			if (compare_bound(&e1[i], &e2[j]) < 0)
				result[rCount++] = e1[i++];
			else
				result[rCount++] = e2[j++];
		}
		else if (i >= nE1 && j < nE2)
			result[rCount++] = e2[j++];
		else if (i < nE1 && j >= nE2)
			result[rCount++] = e1[i++];
	}
}

static void get_prim_nums_for_leaf(int* prims, int nPrimsMax, BoundEdge* edges, int edges_count, 
	int *extreme_indexes[2][3], int extreme_indexes_count[2][3])
{
	int i, j, k, nPrims = 0;
	int *checked_prims = (int *)calloc(nPrimsMax, sizeof(int));

	for (i = 0; i < edges_count; i++)
	{
		if (!checked_prims[edges[i].primNum])
		{
			prims[nPrims++] = edges[i].primNum;
			checked_prims[edges[i].primNum] = 1;
		}
	}
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 3; j++)
		{
			for (k = 0; k < extreme_indexes_count[i][j]; k++)
			{
				int index_of_prim = extreme_indexes[i][j][k];
				if (!checked_prims[index_of_prim])
				{
					prims[nPrims++] = index_of_prim;
					checked_prims[index_of_prim] = 1;
				}
			}
		}
	}
	free(checked_prims);
}

// build tree �Լ��� tree node���� �ҷ������Ƿ� 
// ���׸��� ���� ������带 �����ؾ� �Ѵ�.
static void build_tree(KDAccelTree *kdtree, KDAccelNode *current_node, BBox *nodeBounds, BBox *prim_bounds, int total_prim_counts, int depth, 
	BoundEdge *edges, int edges_count, int *extreme_indexes[2][3], int extreme_indexes_count[2][3], float extreme_bounds[2][3], int badRefines)
{	
	// kdtree�� �ڽ� ��� ����
	KDAccelNode *below_child, *above_child;
	// �ڽ� ����� boundary box
	BBox bbox_below, bbox_above;

	// ���� cost
	float oldCost = (float)kdtree->isectCost * total_prim_counts;
	// total SA�� ������
	float invTotalSA = 1.f / get_surface_of_bbox(nodeBounds);
	// ���� cost
	float bestCost = FLT_MAX;
	// diagonal vector
	float d[3] = {
		nodeBounds->faaBounds[1][0] - nodeBounds->faaBounds[0][0],
		nodeBounds->faaBounds[1][1] - nodeBounds->faaBounds[0][1],
		nodeBounds->faaBounds[1][2] - nodeBounds->faaBounds[0][2]
	};

	int bestNAbove = 0, bestNBelow = 0, bestSide = BELOW;
	int nBelow[3], nPlanar[3], nAbove[3];
	BoundEdge bestPlane = { 0 };
	BoundEdge *leftEdges, *rightEdges;
	int nLeftEdges, nRightEdges;
	int *primNums;
	int *extreme_for_above[2][3];
	int extreme_count_for_above[2][3];
	int *extreme_for_below[2][3];
	int extreme_count_for_below[2][3];
	int i, j, k;
		
	// �ڽ� ��带 ���� ���� Ȯ��
	allocChild(kdtree);

	// ���� ���ǿ� �ٴٸ��� ���� ���� ����
	if (total_prim_counts <= kdtree->maxPrims || depth == 0)
	{
		primNums = (int*)malloc(total_prim_counts * sizeof(int));
		get_prim_nums_for_leaf(primNums, kdtree->nPrims, edges, edges_count, extreme_indexes, extreme_indexes_count);
		initLeaf(kdtree, current_node, primNums, total_prim_counts);
		free(primNums);
		return;
	}

	{
		// bestPlane �ʱ�ȭ
		bestPlane.axis = -1;

		// ������� �������� ������ ���� prim ���� �ʱ�ȭ 
		for (i = 0; i < 3; i++)
		{
			nAbove[i] = total_prim_counts - extreme_indexes_count[0][i];
			nPlanar[i] = 0;
			nBelow[i] = extreme_indexes_count[0][i];
		}

		// edge�� ����� ����� ������ �� cost�� �ּҰ� �Ǵ� ��� ����
		i = 0;
		while (i < edges_count)
		{
			BoundEdge curPlane = edges[i];
			float minBound = max(nodeBounds->faaBounds[0][curPlane.axis], extreme_bounds[0][curPlane.axis]);
			float maxBound = min(nodeBounds->faaBounds[1][curPlane.axis], extreme_bounds[1][curPlane.axis]);
			int nEndOfCurPlane = 0, nPlanarOfCurPlane = 0, nStartOfCurPlane = 0;

			// ���� ��鿡 ��� �ִ� edge���� �з���
			while (i < edges_count && edges[i].axis == curPlane.axis && edges[i].t == curPlane.t)
			{				
				switch (edges[i++].e_type)
				{
				case END:		
					nEndOfCurPlane++;
					break;
				case PLANAR:
					nPlanarOfCurPlane++;					
					break;
				case START: 	
					nStartOfCurPlane++;
					break;
				}				
			}
			nAbove[curPlane.axis] -= (nEndOfCurPlane + nPlanarOfCurPlane);
			nPlanar[curPlane.axis] = nPlanarOfCurPlane;

			// �ڽ�Ʈ ���
			if (minBound < curPlane.t && curPlane.t < maxBound)
			{
				float cost;
				int side;

				// �� ��� ���� ���� �ִ� edge�� ���� cost ���
				int otherAxis0 = (curPlane.axis + 1) % 3, otherAxis1 = (curPlane.axis + 2) % 3;
				float pBelow = 2 * (
					(d[otherAxis0] * d[otherAxis1]) + (curPlane.t - minBound) * (d[otherAxis0] + d[otherAxis1])
					) * invTotalSA;
				float pAbove = 2 * (
					(d[otherAxis0] * d[otherAxis1]) + (maxBound - curPlane.t) * (d[otherAxis0] + d[otherAxis1])
					) * invTotalSA;

				int nCurB = nBelow[curPlane.axis];
				int nCurA = nAbove[curPlane.axis];
				int nCurP = nPlanar[curPlane.axis];

				cost = getCost(kdtree, nCurB, nCurP, nCurA, pBelow, pAbove, &side);

				// �ּ� �ڽ�Ʈ��� ����
				if (cost < bestCost)
				{
					bestPlane = curPlane;
					bestCost = cost;
					bestSide = side;
					bestNBelow = nCurB;
					bestNAbove = nCurA;

					if (side == BELOW)	bestNBelow += nCurP;
					else				bestNAbove += nCurP;

					DEBUG_ONLY(if (bestCost < 0)
					{
						PDEBUG("WARN: cost %f, nBelow %d, pBelow %.2f, nAbove %d, pAbove %.2f\n",
							bestCost, nCurB, pBelow, nCurA, pAbove);
						PAUSE;
					});
				}
			}

			// ���� loop�� ���� ���� ���� 
			nPlanar[curPlane.axis] = 0;
			nBelow[curPlane.axis] += (nPlanarOfCurPlane + nStartOfCurPlane);			
		} // while
	}
	if (bestCost > oldCost) badRefines++;

	// ������ ���� ������ ��ġ�� ������ ���� ��� ����
	if ((bestCost > 4.f * oldCost && total_prim_counts < 16) || bestPlane.axis == -1 || badRefines >= 3)
	{
		primNums = (int*)malloc(total_prim_counts * sizeof(int));
		get_prim_nums_for_leaf(primNums, kdtree->nPrims, edges, edges_count, extreme_indexes, extreme_indexes_count);
		initLeaf(kdtree, current_node, primNums, total_prim_counts);
		free(primNums);
		return;
	}


	// above node�� below node�� ������ extreme prims�� ������
	// �ʱ�ȭ
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 3; j++)
		{
			extreme_for_above[i][j] = (int *)malloc(extreme_indexes_count[i][j] * sizeof(int));
			extreme_count_for_above[i][j] = 0;
			extreme_for_below[i][j] = (int *)malloc(extreme_indexes_count[i][j] * sizeof(int));
			extreme_count_for_below[i][j] = 0;
		}
	}

	// bestPlane�� �������� edges�� left, right �� �׷����� ������ ����
	{
		BoundEdge *leftOnlys, *rightOnlys, *bothLefts, *bothRights;
		int *location_of_prims;
		int nLO = 0, nRO = 0, nBL = 0, nBR = 0;

		leftOnlys = (BoundEdge *)malloc(edges_count * sizeof(BoundEdge));
		rightOnlys = (BoundEdge *)malloc(edges_count * sizeof(BoundEdge));
		bothLefts = (BoundEdge *)malloc(edges_count * sizeof(BoundEdge));
		bothRights = (BoundEdge *)malloc(edges_count * sizeof(BoundEdge));
		location_of_prims = (int *)malloc(kdtree->nPrims * sizeof(int));

		// edge�� ���� primitive�� ��� �ʿ� ���ϴ��� ����		
		for (i = 0; i < kdtree->nPrims; i++){
			location_of_prims[i] = BOTH;
		}

		// extrem prims ����
		for (i = 0; i < 2; i++)
		{
			for (j = 0; j < 3; j++)
			{
				for (k = 0; k < extreme_indexes_count[i][j]; k++)
				{
					int axis = bestPlane.axis;
					int prim_index = extreme_indexes[i][j][k];
					float min_bound_of_prim = prim_bounds[prim_index].faaBounds[0][axis];
					float max_bound_of_prim = prim_bounds[prim_index].faaBounds[1][axis];

					// extreme prim�� ���� ��� �ؿ� �ִ� ���
					if (max_bound_of_prim <= bestPlane.t)
					{
						extreme_for_below[i][j][extreme_count_for_below[i][j]++] = prim_index;
						location_of_prims[prim_index] = BELOW;
					}
					// extreme prim�� ���� ��� ���� �ִ� ���
					else if (min_bound_of_prim >= bestPlane.t)
					{
						extreme_for_above[i][j][extreme_count_for_above[i][j]++] = prim_index;
						location_of_prims[prim_index] = ABOVE;
					}
					// extreme prim�� ���� ��� ���ʿ� �ִ� ���
					else
					{
						extreme_for_below[i][j][extreme_count_for_below[i][j]++] = prim_index;
						extreme_for_above[i][j][extreme_count_for_above[i][j]++] = prim_index;
					}
				}
			}
		}
		
		// ���� edge�� ���� 
		for (i = 0; i < edges_count; i++)
		{
			if (edges[i].axis == bestPlane.axis)
			{
				switch (edges[i].e_type)
				{
				case END:
					if (edges[i].t <= bestPlane.t)
						location_of_prims[edges[i].primNum] = BELOW;
					break;
				case START:
					if (edges[i].t >= bestPlane.t)
						location_of_prims[edges[i].primNum] = ABOVE;
					break;
				case PLANAR:
					if (edges[i].t < bestPlane.t || (edges[i].t == bestPlane.t && bestSide == BELOW))
						location_of_prims[edges[i].primNum] = BELOW;
					if (edges[i].t > bestPlane.t || (edges[i].t == bestPlane.t && bestSide == ABOVE))
						location_of_prims[edges[i].primNum] = ABOVE;
					break;
				}
			}
		}

		// �߰� �迭�� �з��� edge�� ����
		for (i = 0; i < edges_count; i++)
		{
			switch (location_of_prims[edges[i].primNum])
			{
			case BELOW:
				leftOnlys[nLO++] = edges[i];
				break;
			case ABOVE:
				rightOnlys[nRO++] = edges[i];
				break;
			case BOTH:
				if (edges[i].axis != bestPlane.axis)
				{
					bothLefts[nBL++] = edges[i];
					bothRights[nBR++] = edges[i];
				}
				else if (edges[i].e_type == START)
				{
					BoundEdge newEnd;
					init_bound_edge(&newEnd, bestPlane.t, edges[i].primNum, END, bestPlane.axis);

					bothLefts[nBL++] = edges[i];
					bothLefts[nBL++] = newEnd;
				}
				else if (edges[i].e_type == END)
				{
					BoundEdge newStart;
					init_bound_edge(&newStart, bestPlane.t, edges[i].primNum, START, bestPlane.axis);

					bothRights[nBR++] = newStart;
					bothRights[nBR++] = edges[i];
				}
				break;
			}
		}
		// both left, right �迭 ������ O(��n(log��n))
		qsort(bothLefts, nBL, sizeof(BoundEdge), compare_bound);
		qsort(bothRights, nBR, sizeof(BoundEdge), compare_bound);

		// �迭���� ����
		nLeftEdges = nLO + nBL;
		nRightEdges = nRO + nBR;
		leftEdges = (BoundEdge*)malloc(nLeftEdges * sizeof(BoundEdge));
		rightEdges = (BoundEdge*)malloc(nRightEdges * sizeof(BoundEdge));
		merge_bound(leftEdges, leftOnlys, bothLefts, nLO, nBL);
		merge_bound(rightEdges, rightOnlys, bothRights, nRO, nBR);

		free(leftOnlys);
		free(rightOnlys);
		free(bothLefts);
		free(bothRights);
		free(location_of_prims);
	}

	PDEBUG("nlogn build_tree depth %d, cost %f, below %d, above %d\n", 10 - depth, bestCost, nLeftEdges, nRightEdges);

	// ��������� �ڽ� ��� �ʱ�ȭ
	bbox_below = *nodeBounds;
	bbox_above = *nodeBounds;
	bbox_below.faaBounds[0][bestPlane.axis] = bestPlane.t;
	bbox_above.faaBounds[1][bestPlane.axis] = bestPlane.t;

	below_child = &kdtree->nodes[kdtree->nextFreeNodes];
	build_tree(kdtree, below_child, &bbox_below, prim_bounds,
		bestNBelow, depth - 1, leftEdges, nLeftEdges, extreme_for_below, extreme_count_for_below, extreme_bounds, badRefines);

	above_child = &kdtree->nodes[kdtree->nextFreeNodes];
	build_tree(kdtree, above_child, &bbox_above, prim_bounds,
		bestNAbove, depth - 1, rightEdges, nRightEdges, extreme_for_above, extreme_count_for_above, extreme_bounds, badRefines);

	initInterior(current_node, above_child, below_child, bestPlane.axis, bestPlane.t);

	free(leftEdges);
	free(rightEdges);
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 3; j++)
		{
			free(extreme_for_above[i][j]);
			free(extreme_for_below[i][j]);
		}
	}
}

static void initTree(KDAccelTree *kdtree, Primitive* p)
{
	// ��� �ĺ� �迭�� ������  ����
	BoundEdge *edges;
	int edges_count;
	int *extreme_indexes[2][3];
	int extreme_indexes_count[2][3];
	float extreme_bounds[2][3];
	// primitive ����
	int prims_count = kdtree->nPrims;
	// primitive�� bound �迭�� ������ ����
	BBox *prim_bounds;
	// primitive�� index �迭�� ������ ����
	int *prim_indexes;
	int i;

	// Ʈ���� ��� 512�� �Ҵ� �� ����ִ� ��� ��ȣ ����
	kdtree->nextFreeNodes = 0;
	kdtree->nAllocednodes = 512;
	kdtree->nodes = (KDAccelNode *)mzalloc(sizeof(KDAccelNode) * 512);

	// Ʈ���� �ִ� ���̸� ������ 
	if (kdtree->maxDepth <= 0)
	{
		//TODO: �ùٸ� ��갪���� ���ľ���
		kdtree->maxDepth = 5;
	}

	// prim_indexes(��忡 ����ִ� ������Ƽ���� �ε��� ����) �ʱ�ȭ 
	prim_indexes = (int *)malloc(sizeof(int) * prims_count);
	for (i = 0; i < prims_count; i++){
		prim_indexes[i] = i;
	}

	// kdtree�� ��ü bound ��� �� �� primitive�� bound�� ���
	prim_bounds = (BBox *)malloc(sizeof(BBox) * prims_count);
	kdtree->bounds = make_bbox_by_triangle(kdtree->primitives[0]);

	for (i = 0; i < prims_count; i++)
	{
		BBox b = make_bbox_by_triangle(kdtree->primitives[i]);
		// ��ü bound ũ�� ����
		kdtree->bounds = union_bbox_and_bbox(kdtree->bounds, b);
		// primitive bound ����
		prim_bounds[i] = b;
	}	

	// ��� �迭�� ����, ����
	edges = (BoundEdge *)malloc(sizeof(BoundEdge) * prims_count * 2 * 3);
	edges_count = 0;
	for (i = 0; i < 3; i++){
		extreme_indexes[0][i] = (int *)malloc(sizeof(int) * prims_count);
		extreme_indexes[1][i] = (int *)malloc(sizeof(int) * prims_count);
		extreme_indexes_count[0][i] = 0;
		extreme_indexes_count[1][i] = 0;
		extreme_bounds[0][i] = (kdtree->bounds.faaBounds[1][i] - kdtree->bounds.faaBounds[0][i]) * PERCENT + kdtree->bounds.faaBounds[0][i];
		extreme_bounds[1][i] = (kdtree->bounds.faaBounds[1][i] - kdtree->bounds.faaBounds[0][i]) * (1 - PERCENT) + kdtree->bounds.faaBounds[0][i];
	}
	
	for (i = 0; i < prims_count; i++)
	{
		int prim_index = prim_indexes[i], axis;
		BBox bbox = prim_bounds[prim_index];

		for (axis = 0; axis < 3; axis++)
		{
			float bbox_min = bbox.faaBounds[0][axis], bbox_max = bbox.faaBounds[1][axis];

			if (bbox_max <= extreme_bounds[0][axis])
			{
				extreme_indexes[0][axis][extreme_indexes_count[0][axis]++] = i;
			}				
			else if (bbox_min >= extreme_bounds[1][axis])
			{
				extreme_indexes[1][axis][extreme_indexes_count[1][axis]++] = i;
			}
			else if (bbox_min == bbox_max)
			{
				init_bound_edge(&edges[edges_count++], bbox_min, prim_index, PLANAR, axis);
			}
			else
			{
				init_bound_edge(&edges[edges_count++], bbox_min, prim_index, START, axis);
				init_bound_edge(&edges[edges_count++], bbox_max, prim_index, END, axis);
			}
		}
	}
	qsort(edges, edges_count, sizeof(BoundEdge), compare_bound);	

	// kdtree ������ ���� ��͹� ����
	build_tree(kdtree, &kdtree->nodes[0], &kdtree->bounds, prim_bounds, kdtree->nPrims, kdtree->maxDepth, 
		edges, edges_count, extreme_indexes, extreme_indexes_count, extreme_bounds, 0);

	// kdtree ���࿡ ����� ���� ����
	free(prim_indexes);
	free(prim_bounds);
	free(edges);
	for (i = 0; i < 3; i++){
		free(extreme_indexes[0][i]);
		free(extreme_indexes[1][i]);
	}
}

void minmax_accel_build(Data *data)
{
	KDAccelTree *kdtree = (KDAccelTree *)mzalloc(sizeof(KDAccelTree));
	data->accel_struct = (void *)kdtree;

	kdtree->isectCost = 80;
	kdtree->traversalCost = 1;
	kdtree->emptyBonus = 0.5f;
	kdtree->maxPrims = 1;
	kdtree->maxDepth = 10;
	kdtree->nPrims = data->prim_count;

	kdtree->primitives = (Primitive *)malloc(sizeof(data->primitives[0]) * data->prim_count);
	memcpy(kdtree->primitives, data->primitives, sizeof(Primitive)*data->prim_count);

	initTree(kdtree, data->primitives);
}
