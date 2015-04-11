
/*
Copyright (c) 2015, Daeyoun Kang(mail.ikasty@gmail.com),
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

#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "include/bitmap.h"
#include "include/debug-msg.h"

#define SWAP16(x) (((((x) & 0xFF) << 8 | (((x) >> 8) & 0xff))))

void OutputFrameBuffer(int res_x, int res_y, int* FB, char* file_name)
{
	BITMAPFILEHEADER FileHeader;
	BITMAPINFOHEADER ImageHeader;

	FILE *fp;

	fp = fopen(file_name,"wb");
	
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
	fwrite(&FileHeader, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
        fwrite(&ImageHeader, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(FB, sizeof(DWORD), res_x * res_y, fp);
	fclose(fp);
}
