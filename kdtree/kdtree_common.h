#ifndef KDTREE_COMMON_H
#define KDTREE_COMMON_H

#include "kdtree_type.h"

void init_bound_edge(BoundEdge* bd, float tt, int pn, int t, int a);
void initLeaf(KDAccelTree *kdtree, KDAccelNode *node, int *prim_indexes, int np);
void initInterior(KDAccelNode *node, KDAccelNode *ac, KDAccelNode *bc, int axis, float s);
void allocChild(KDAccelTree *kdtree);
float getCost(KDAccelTree *kdtree, int nBelow, int nPlanar, int nAbove, float pBelow, float pAbove, int *side);

#endif
