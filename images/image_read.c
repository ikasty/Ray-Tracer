// image_read.c
#include "image_read.h"

// 이미지 파일을 읽어서 포맷에 맞게 처리하는 함수
int image_read(Image *image, char *filename, int flags){
 int (*func)(Image*, char*, int) = NULL;
 const char *ext = strrchr(filename, '.');
	ext = ext + 1;
	// 파일 이름 문자열 분석
 	// - 확장자가 없으면 처리하지 않음
	if (!ext || ext == filename)
	{
		return -1;
	}
	// 알맞게 처리 함수를 정하기
	if (!strcmp(ext, "jpg") || !strcmp(ext, "jpeg"))
	{
		func = jpeg_read;
	}
	if (!strcmp(ext, "png"))
	{
		func = png_read;
	}
	if (!strcmp(ext, "gif"))
	{
		func = gif_read;
	}
	if (!strcmp(ext, "tiff"))
	{
		func = tiff_read;
	}
	if (!strcmp(ext, "bmp"))
	{
		func = bmp_read;
	}

 // 확장자에 따른 방법으로 처리 수행 
 // * flags 미구현
 return (func != NULL)?func(image, filename, IMAGE_NO_FLAGS):-1;
}

// 이미지 파일을 읽어서 포맷에 맞게 처리하는 함수
int image_write(Image *image, char *filename, int flags){
 int (*func)(Image*, char*, int) = NULL;
 const char *ext = strrchr(filename, '.');
	ext = ext + 1;
 //PDEBUG("image write start\n");
 //PDEBUG("%s\n", filename);
 //PDEBUG("%s\n", ext);
	// 파일 이름 문자열 분석
 	// - 확장자가 없으면 처리하지 않음
	if (!ext || ext == filename)
	{
		return -1;
	}
	// 알맞게 처리 함수를 정하기
	if (strcmp(ext, "jpg") || strcmp(ext, "jpeg"))
	{
		func = jpeg_write;
	}
/*	if (strcmp(ext, "png"))
	{
		func = png_read;
	}
	if (strcmp(ext, "gif"))
	{
		func = gif_read;
	}
	if (strcmp(ext, "tiff"))
	{
		func = tiff_read;
	}*/
	if (!strcmp(ext, "bmp"))
	{
  //PDEBUG("select bmp write\n");
		func = bmp_write;
	}

 // 확장자에 따른 방법으로 처리 수행 
 // * flags 미구현
 //return func(image, filename, IMAGE_NO_FLAGS);
 return (func != NULL)?func(image, filename, IMAGE_NO_FLAGS):-1;
}
