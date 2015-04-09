#ifndef KDTREE_COMMON_H
#define KDTREE_COMMON_H

#include "kdtree_type.h"

void initLeaf(KDAccelTree *kdtree, KDAccelNode *node, int *prim_indexes, int np);
void initInterior(KDAccelNode *node, KDAccelNode *ac, KDAccelNode *bc, int axis, float s);
void allocChild(KDAccelTree *kdtree);

#endif
