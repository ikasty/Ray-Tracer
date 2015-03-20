#ifndef BITMAP_MAKE_H
#define BITMAP_MAKE_H
#include "bitmap.h"
void OutputFrameBuffer(int res_x, int res_y, int* FB, char* file_name);

typedef struct  
{
	unsigned int *FrameBuffer; //프레임버퍼
	int resx, resy; //해상도
}Shader;

//typedef unsigned char  BYTE;
//typedef unsigned short WORD;
//typedef unsigned long  DWORD;
//typedef long           LONG;  

typedef struct{
    WORD    bfType;
    DWORD   bfSize;
    WORD    bfReserved1;
    WORD    bfReserved2;
    DWORD   bfOffBits;

} _BITMAPFILEHEADER;

typedef struct{
    DWORD  biSize;
    LONG   biWidth;
    LONG   biHeight;
    WORD   biPlanes;
    WORD   biBitCount;
    DWORD  biCompression;
    DWORD  biSizeImage;
    LONG   biXPelsPerMeter;
    LONG   biYPelsPerMeter;
    DWORD  biClrUsed;
    DWORD  biClrImportant;

} _BITMAPINFOHEADER;

#endif
