/*******************************
 * header for cross compile
 * Daeyoun Kang
 * last modyfied at 2015-03-02
 ******************************/

#ifndef CROSS_COMPILE_H
#define CROSS_COMPILE_H

#include <errno.h>
#include <string.h>
#include <stdint.h>

////////////////////////////
// safer count macro
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
#	define DEBUG 10
#	else
#	undef DEBUG
#	endif
#endif

////////////////////////////
// debug print module
#ifdef DEBUG
#define PDEBUG(l, ...) if (DEBUG >= l) {			\
	int __i;										\
	printf("DEBUG] ");								\
	for(__i = 0; __i < l - 1; __i++) printf("  ");	\
	printf(__VA_ARGS__);							\
}
#define PAUSE if (DEBUG >= 3) getchar()
#define DEBUG_ONLY(...) __VA_ARGS__
#else
#define PDEBUG(l, ...) (0)
#define PAUSE (0)
#define DEBUG_ONLY(...)
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