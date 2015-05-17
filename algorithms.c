#include <stdio.h>
#include <string.h>
#include "algorithms.h"

////////////////////////////////////////
// search algorithms
// 1. NAIVE
// * naive algorithm
#  include "naive/naive_intersection.h"

// KDTREE
// * kdtree_clear_accel, kdtree_intersect_search
#  include "kdtree/kdtree_intersect_clear.h"
// * nlog^2n algorithm
#  include "kdtree/nlog2n_build.h"
// * nlogn algorithm
#  include "kdtree/nlogn_build.h"
// * binning algorithm
#  include "kdtree/binning_build.h"
// * nlogn exclusion algorithm
#  include "kdtree/exclusion_build.h"

/////////////////////////////////////
// shading algorithms
// 1. naive
#  include "shading/naive_shading.h"
// 2. phong shading
#  include "shading/phong_shading.h"

// default values
void (*clear_accel)(Data *data) = NULL;
void (*accel_build)(Data *data) = NULL;
Hit (*intersect_search)(Data *data, Ray *ray) = NULL;

void init_search_algo(char *algo_name)
{
	// 이미 알고리즘이 선택된 경우 무시
	if (intersect_search) return ;

	if (strncmp(algo_name, "naive", 5) == 0)
	{
		printf("use naive algorithm\n");
		clear_accel = NULL;
		accel_build = NULL;
		intersect_search = &naive_intersect_search;
	}
	else if (strncmp(algo_name, "nlog2n", 6) == 0)
	{
		printf("use kdtree nlog^2n algorithm\n");
		clear_accel = &kdtree_clear_accel;
		accel_build = &nlog2n_accel_build;
		intersect_search = &kdtree_intersect_search;
	}
	else if (strncmp(algo_name, "nlogn", 5) == 0)
	{
		printf("use kdtree nlogn algorithm\n");
		clear_accel = &kdtree_clear_accel;
		accel_build = &nlogn_accel_build;
		intersect_search = &kdtree_intersect_search;
	}
	else if (strncmp(algo_name, "binning", 7) == 0)
	{
		printf("use kdtree binning algorighm\n");
		clear_accel = &kdtree_clear_accel;
		accel_build = &binning_accel_build;
		intersect_search = &kdtree_intersect_search;
	}
	else if (strncmp(algo_name, "exclusion", 9) == 0)
	{
		printf("use kdtree nlogn exclusion algorithm\n");
		clear_accel = &kdtree_clear_accel;
		accel_build = &exclusion_accel_build;
		intersect_search = &kdtree_intersect_search;
	}
	else
	{
		printf("use default(kdtree nlogn) algorithm\n");
		clear_accel = &kdtree_clear_accel;
		accel_build = &nlogn_accel_build;
		intersect_search = &kdtree_intersect_search;
	}

	return ;
}

void (*normal_shade)(float normal_vector[3], float hit_point[3], Primitive s_tri) = NULL;

void init_shading_algo(char *shading_name)
{
	if (strncmp(shading_name, "naive", 5) == 0)
	{
		printf("use naive shading\n");
		normal_shade = &naive_shading;
	}
	else if (strncmp(shading_name, "phong", 5) == 0)
	{
		printf("use phong shading\n");
		normal_shade = &phong_shading;
	}
	else
	{
		printf("use default(phong) shading\n");
		normal_shade = &phong_shading;
	}	
	return ;
}