#ifndef KDTREE_BUILD_H
#define KDTREE_BUILD_H

#include "kdtree_type.h"

void nlog2n_accel_build(Data *data);
void initLeaf(KDAccelTree *kdtree, KDAccelNode *node, int *prim_indexes, int np);
void initInterior(KDAccelNode *node, KDAccelNode *ac, KDAccelNode *bc, int axis, float s);

#endif