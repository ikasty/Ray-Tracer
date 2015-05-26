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
