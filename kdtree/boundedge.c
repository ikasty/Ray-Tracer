﻿#include "boundedge.h"

void init_bound_edge(BoundEdge* bd, float tt, int pn, int t){
	bd->t = tt;
	bd->primNum = pn;
	bd->e_type = t;
}