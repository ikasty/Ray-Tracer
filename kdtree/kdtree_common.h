#ifndef KDTREE_COMMON_H
#define KDTREE_COMMON_H

#include "kdtree_type.h"

void init_bound_edge(BoundEdge* bd, float tt, int pn, int t, int a);
void initLeaf(KDAccelTree *kdtree, int node_idx, int *prim_indexes, int np);
void initInterior(KDAccelTree *kdtree, int node_idx, int above_child_idx, int below_child_idx, int axis, float s);
void allocChild(KDAccelTree *kdtree);
float getCost(KDAccelTree *kdtree, int nBelow, int nPlanar, int nAbove, float pBelow, float pAbove, int *side);

DEBUG_ONLY(
extern int leaf_count;
extern int leaf_prim_count;
extern int leaf_max_prim_count;

void leaf_info_print();
);

#endif
