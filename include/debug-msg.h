
/*
Copyright (c) 2015, Daeyoun Kang(mail.ikasty@gmail.com)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies, 
either expressed or implied, of the FreeBSD Project.
*/

#ifndef DEBUG_MSG_H
#define DEBUG_MSG_H

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

////////////////////////////
// safer array count macro
#define countof(x) ( sizeof(0[x]) == 0 ? 0 : ((sizeof(x)/sizeof(0[x]))) / ((size_t)(!(sizeof(x)%(sizeof(0[x]))))) )

////////////////////////////
// code compatibility
#ifdef _WIN32
#	define inline
#	include <windows.h>
#	define mkdir CreateDirectory
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
	
#	define PDEBUG(...) 	{			\
		if (__PDEBUG_ENABLED != 1)	\
			printf("\n");			\
		__PDEBUG_ENABLED = 1;		\
		printf("DEBUG] ");			\
		printf(__VA_ARGS__);		\
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