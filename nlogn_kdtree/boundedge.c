#include "boundedge.h"

void init_nlogn_bound_edge(BoundEdge* bd, float tt, int pn, int t, int a);

void init_nlogn_bound_edge(BoundEdge* bd, float tt, int pn, int t, int a){
	bd->t = tt;
	bd->primNum = pn;
	bd->e_type = t;
	bd->axis = a;
}