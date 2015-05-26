#ifndef IMAGE_BMP_H
#define IMAGE_BMP_H

#include "image_buffer.h"
#include "include/images/bitmap.h"

#define SWAP16(x) (((((x) & 0xFF) << 8 | (((x) >> 8) & 0xff))))

//typedef struct
//{
//  unsigned int *FrameBuffer; //프레임버퍼
//   int resx, resy; //해상도
//} Shader;

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


int bmp_read(Image *image, char *filename, int flags);
int bmp_write(Image *image, char *filename, int flags);

#endif
