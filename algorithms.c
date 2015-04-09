#include <string.h>
#include "algorithms.h"

////////////////////////////////////////
// search algorithms
// 1. naive
#  include "naive/naive_intersection.h"
// 2. kdtree(nlog^2n)
#  include "kdtree/nlog2n_build.h"
#  include "kdtree/nlog2n_intersection.h"
// 3. kdtree(nlogn)
#  include "kdtree/nlogn_build.h"

/////////////////////////////////////
// shading algorithms
// 1. naive
#  include "naive/naive_shading.h"

// default values
void (*clear_accel)(Data *data) = &nlog2n_clear_accel;
void (*accel_build)(Data *data) = &nlog2n_accel_build;
Hit (*intersect_search)(Data *data, Ray *ray) = &nlog2n_intersect_search;

void init_search_algo(char *algo_name)
{
	if (strncmp(algo_name, "naive", 5) == 0)
	{
		clear_accel = NULL;
		accel_build = NULL;
		intersect_search = &naive_intersect_search;
	}
	else if (strncmp(algo_name, "kdtree", 6) == 0)
	{
		clear_accel = &nlog2n_clear_accel;
		accel_build = &nlog2n_accel_build;
		intersect_search = &nlog2n_intersect_search;
	}
	else if (strncmp(algo_name, "nlogn_kdtree", 6) == 0)
	{
		clear_accel = &nlog2n_clear_accel;
		accel_build = &nlog2n_accel_build;
		intersect_search = &nlog2n_intersect_search;
	}

	return ;
}

unsigned int (*shading)(Ray s_ray, Primitive s_tri, Hit __hit) = &naive_shading;

void init_shading_algo(char *shading_name)
{
	return ;
}