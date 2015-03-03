/////////////////////////////////////////////////
// sojong project
// 2015 spring
//
// Daeyoun Kang, Ingyu Kim, Jungmin Kim
/////////////////////////////////////////////////

///////////////////////
// include headers
#include <stdio.h>

// debug settings must place before cross-compile.h
#undef DEBUG

// cross compile and debug settings
#include "cross-compile.h"

int main()
{
	DEBUG_ONLY(
		printf("* Debug mode enabled!\n");
	);
}