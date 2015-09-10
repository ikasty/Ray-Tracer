
/*
Copyright (c) 2015, Minwoo Lee(hellomw@msl.yonsei.ac.kr)
					Daeyoun Kang(mail.ikasty@gmail.com),
                    HyungKwan Park(rpdladps@gmail.com),
                    Ingyu Kim(goracom0@gmail.com),
                    Jungmin Kim(kukakhan@gmail.com)
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

#ifndef BITMAP_H
#define BITMAP_H
#include "include/type.h"

#ifdef _WIN32
#pragma pack(push,2)
#else
# if !(defined(lint) || defined(RC_INVOKED))
# pragma pack(push,2)
# endif
#endif

#if (__GNUC__ == 4) && (__GNUC_MINOR__ >= 7) && defined(__MINGW32__)
#pragma ms_struct off
#endif

#ifdef _WIN32
struct tagBITMAPFILEHEADER {
#else
struct __attribute__ ((aligned (2), packed, ms_struct)) tagBITMAPFILEHEADER {
#endif
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
};
typedef struct tagBITMAPFILEHEADER BITMAPFILEHEADER;

#ifdef _WIN32
struct tagBITMAPINFOHEADER{
#else
struct __attribute__((aligned(1), packed, ms_struct)) tagBITMAPINFOHEADER{
#endif
	DWORD      biSize;
	LONG       biWidth;
	LONG       biHeight;
	WORD       biPlanes;
	WORD       biBitCount;
	DWORD      biCompression;
	DWORD      biSizeImage;
	LONG       biXPelsPerMeter;
	LONG       biYPelsPerMeter;
	DWORD      biClrUsed;
	DWORD      biClrImportant;
};
typedef struct tagBITMAPINFOHEADER BITMAPINFOHEADER;

#ifdef _WIN32
#pragma pack(pop)
#else
# if !(defined(lint) || defined(RC_INVOKED))
# pragma pack(pop)
# endif
#endif

/* constants for the biCompression field */
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
#define BI_JPEG       4L
#define BI_PNG        5L

#endif
