#ifndef KDTREE_BUILD_H
#define KDTREE_BUILD_H

#include "kdtree_type.h"

void kdtree_accel_build(Data *data);
static int compare(const void* a, const void* b);
static void initTree(KDAccelTree *kdtree, Primitive* p, int np, int icost, int tcost, 
			  float ebonus, int maxp, int md);
static void initLeaf(KDAccelTree* kdtree, KDAccelNode* node, int* primNums, int np);
static void initInterior(KDAccelTree* kdtree, KDAccelNode* node, int axis, int ac, int bc, float s);
static void buildTree(KDAccelTree *kdtree, int nodeNum, BBox *nodeBounds, 
			   BBox *allPrimBounds, int *primNums, int nPrimitives,int depth, 
			   BoundEdge *edges[3], int *prims0, int *prims1, int badRefines);

#endif