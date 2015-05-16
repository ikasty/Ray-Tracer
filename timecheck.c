#include "timecheck.h"

// for main.c
double build_clock = 0.0, search_clock = 0.0, render_clock = 0.0;

//for algorithms
double intersect_clock = 0.0, shadow_search_clock = 0.0;

/*
INLINE double get_total_clock()
{
	return build_clock + search_clock + render_clock;
}

INLINE double get_build_clock()
{
	return build_clock;
}
INLINE double get_search_clock()
{
	return search_clock + shadow_search_clock - intersect_clock;
}
INLINE double get_render_clock()
{
	return render_clock - shadow_search_clock;
}
*/