#include <stdio.h>
#include <string.h>
#include <math.h>

#include "msl_math.h"

#include "type.h"




hit intersect_triangle(msl_ray ray, triangle triangle)
{
	

	float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
	float det,inv_det;
	static float t,u,v;
	
	hit ist_hit;
	memset(tvec,0,sizeof(tvec));
	SUB(edge1, triangle.vert1, triangle.vert0);
	SUB(edge2, triangle.vert2, triangle.vert0);
	
	CROSS(pvec, ray.dir,edge2);

	det = DOT(edge1,pvec);

	//컬링을 한다고 가정할 경우
	if(det<EPSILON)
	{
		ist_hit.t=0;
		ist_hit.u =0;
		ist_hit.v =0;
		return ist_hit;
	}

	SUB(tvec,ray.orig,triangle.vert0);

	u = DOT(tvec,pvec);
	if(u < 0.0 || u > det)
	{
		ist_hit.t=0;
		ist_hit.u =0;
		ist_hit.v =0;
		return ist_hit;
	}

	CROSS(qvec,tvec,edge1);

	v=DOT(ray.dir,qvec);
	if(v<0.0|| u+v>det)
	{
		ist_hit.t=0;
		ist_hit.u =0;
		ist_hit.v =0;
		return ist_hit;
	}

	t = DOT(edge2,qvec);
	inv_det = 1.0/det;
	t *= inv_det;
	u *= inv_det;
	v *= inv_det;

	ist_hit.t=t;
	ist_hit.u =u;
	ist_hit.v =v;
	
	return ist_hit;
	
}


