#ifndef TIMECHECK_H
#define TIMECHECK_H

#include <time.h>

extern double build_clock, search_clock, render_clock, intersect_clock, shadow_search_clock;
#define USE_TIMECHECK() clock_t start_clock, end_clock;		// 수행 시간 계산용 clock_t 변수

#define TIMECHECK_START() start_clock = clock()
#define TIMECHECK_END(_clock) end_clock = clock(); _clock += (double)(end_clock - start_clock) / CLOCKS_PER_SEC;

inline double get_total_clock();
inline double get_build_clock();
inline double get_search_clock();
inline double get_render_clock();

#endif