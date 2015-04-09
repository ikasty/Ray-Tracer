/*********************************
 * header for debug msg and stuff
 * Daeyoun Kang
 * last modyfied at 2015-03-23
 ********************************/

#ifndef DEBUG_MSG_H
#define DEBUG_MSG_H

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

////////////////////////////
// safer array count macro
#define countof(x) ((sizeof(x)/sizeof(0[x]))) / ((size_t)(!(sizeof(x)%(sizeof(0[x])))))

////////////////////////////
// code compatibility
#ifdef _WIN32
#	define inline __inline
#else

#endif

////////////////////////////
// debug mode compatibility
#ifdef _WIN32
#	ifdef _DEBUG
#	define DEBUG
#	else
#	undef DEBUG
#	endif
#endif

////////////////////////////
// debug print module
extern int __PDEBUG_ENABLED;
#ifdef DEBUG
	
#	define PDEBUG(...) 	{		\
	  	__PDEBUG_ENABLED = 1;	\
		printf("DEBUG] ");		\
		printf(__VA_ARGS__);	\
	}
#	define PAUSE getchar()
#	define DEBUG_ONLY(...) __VA_ARGS__
#	define PDEBUG_INIT() int __PDEBUG_ENABLED = 0
#else
#	define PDEBUG(...) (0)
#	define PAUSE (0)
#	define DEBUG_ONLY(...)
#	define PDEBUG_INIT()
#endif

//////////////////////////
// functions
inline static void *mzalloc(int size) {
	void *t;
	t = malloc(size);
	memset(t, 0, size);
	return t;
}

inline static void *zfree(void *pnt) {
	free(pnt);
	return NULL;
}
#define ABS(p) ( ((p) < 0) ? -(p) : (p) )
#define SETRAND() srand( (unsigned) time(NULL) )
#define swap(x, y) {	\
	uint8_t swap_temp[sizeof(*x) == sizeof(*y) ? (signed)sizeof(*x) : -1]; \
	if (x != y) {							\
	memcpy(swap_temp, y, 	sizeof(*x));	\
	memcpy(y, x, 			sizeof(*x));	\
	memcpy(x, swap_temp, 	sizeof(*x));	\
	}										\
}


#endif //CROSS_COMPILE_H