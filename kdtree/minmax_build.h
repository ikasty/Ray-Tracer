#ifndef MINMAX_BUILD_H
#define MINMAX_BUILD_H

#include "kdtree_type.h"

/*int compare_nlogn_bound(const void *a, const void *b);
void merge_bound(BoundEdge* result, BoundEdge* e1, BoundEdge* e2, int nE1,int nE2);
void get_prim_nums_from_edges(int* prims, int nPrimsMax, BoundEdge* edges, int nEdges);

void buildNlognTree(KDAccelTree *kdtree, int nodeNum, BBox *nodeBounds,
int nPrimitives, int depth, BoundEdge *edges, int nEdges, int badRefines);
void initNlognTree(KDAccelTree *kdtree, Primitive* p);
*/
void minmax_accel_build(Data *data);

#endif