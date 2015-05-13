#ifndef TIMECHECK_H
#define TIMECHECK_H

#include <time.h>
#include "settings.h"

extern double build_clock, search_clock, render_clock, intersect_clock;
clock_t start_clock, end_clock;		// 수행 시간 계산용 clock_t 변수

#endif