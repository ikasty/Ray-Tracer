//#include "bitmap_make.h"
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define SWAP16(x) (((((x) & 0xFF) << 8 | (((x) >> 8) & 0xff))))
void OutputFrameBuffer(int res_x, int res_y, int* FB, char* file_name)
{
	BITMAPFILEHEADER FileHeader;
	BITMAPINFOHEADER ImageHeader;

	FILE *fp;

	fopen_s(&fp, file_name,"wb");
	
//* FileHeader ****************************************************//
	FileHeader.bfType = 0x424D;
	FileHeader.bfSize = (res_x * res_y * 4) + 54; 
	FileHeader.bfReserved1 = 0x00;
	FileHeader.bfReserved2 = 0x00;
	FileHeader.bfOffBits = 54;
	
//* ImageHeader **************************************************//
	ImageHeader.biSize = 40;
	ImageHeader.biWidth = res_x;
	ImageHeader.biHeight = res_y;
	ImageHeader.biPlanes = 1;
	ImageHeader.biBitCount = 32;	// 24(18) / 32(20) 
	ImageHeader.biCompression = BI_RGB;	// 0 / 3 
	ImageHeader.biSizeImage = (res_x * res_y * 4);
	ImageHeader.biXPelsPerMeter = 0x00;
	ImageHeader.biYPelsPerMeter = 0x00;
	ImageHeader.biClrUsed = 0;
	ImageHeader.biClrImportant = 0;

	FileHeader.bfType = SWAP16(FileHeader.bfType);

	//BMP output
	fwrite(&FileHeader, sizeof(unsigned char), sizeof(BITMAPFILEHEADER), fp);
    fwrite(&ImageHeader, sizeof(unsigned char), sizeof(BITMAPINFOHEADER), fp);
	fwrite(FB, sizeof(unsigned int), res_x * res_y, fp);


	fclose(fp);

}
