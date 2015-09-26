// image_bmp.c
#include "image_bmp.h"

// bmp 포맷 읽기
int bmp_read(Image *image, char *filename, int flags){
 // 미구현
 return -1;
}

// bmp 포맷 쓰기
int bmp_write(Image *image, char *filename, int flags){
 BITMAPFILEHEADER FileHeader;
 BITMAPINFOHEADER ImageHeader;

 FILE *fp;
 //PDEBUG("bmp write start\n");
 fp = fopen(filename, "wb");

//* FileHeader *******************************************//
 FileHeader.bfType = 0x424D;
 FileHeader.bfSize = (image->width * image->height * 4) + 54;
 FileHeader.bfReserved1 = 0x00;
 FileHeader.bfReserved2 = 0x00;
 FileHeader.bfOffBits = 54;

//* ImageHeader *******************************************//
 ImageHeader.biSize = 40;
 ImageHeader.biWidth = image->width;
 ImageHeader.biHeight = image->height;
 ImageHeader.biPlanes = 1;
 ImageHeader.biBitCount = 32; // 24(18) / 32(20) 
 ImageHeader.biCompression = BI_RGB; // 0 / 3 
 ImageHeader.biSizeImage = (image->width * image->height * 4);
 ImageHeader.biXPelsPerMeter = 0x00;
 ImageHeader.biYPelsPerMeter = 0x00;
 ImageHeader.biClrUsed = 0;
 ImageHeader.biClrImportant = 0;

 FileHeader.bfType = SWAP16(FileHeader.bfType);

//BMP output
 fwrite(&FileHeader, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
 fwrite(&ImageHeader, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
 //PDEBUG("DWORD : %d, RGBA : %d\n", sizeof(DWORD), sizeof(RGBA));
 fwrite(image->pixels, sizeof(DWORD), image->width * image->height, fp);
 /*
 for(i = 0; i < image->height; i++)
  fwrite(image->pixels[i], sizeof(DWORD), image->width, fp);
  */

 fclose(fp);

 //PDEBUG("bmp write end\n");
 // TODO: 예외처리 추가좀
 return 0;
}
