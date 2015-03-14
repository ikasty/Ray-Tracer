#ifndef BITMAP_H
#define BITMAP_H

typedef unsigned short      WORD;
typedef unsigned long       DWORD;
typedef long				LONG;

#ifdef _WIN32
#pragma pack(push,2)
#else
#pragma pack(2)
#endif

struct tagBITMAPFILEHEADER {
	WORD    bfType;
	DWORD   bfSize;
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;
#ifdef _WIN32
};
#else
} __attribute__((packed));
#endif

typedef struct tagBITMAPFILEHEADER BITMAPFILEHEADER;

#ifdef _WIN32
#pragma pack(pop)
#else
#pragma pack()
#endif

typedef struct tagBITMAPINFOHEADER{
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
} BITMAPINFOHEADER;

/* constants for the biCompression field */
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
#define BI_JPEG       4L
#define BI_PNG        5L

#endif