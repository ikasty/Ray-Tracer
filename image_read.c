// image_read.c
#include "image_read.h"

// 이미지 파일을 읽어서 포맷에 맞게 처리하는 함수
int image_read(Data *data, char *image_name, int flags){
 int (*func)(Data*, char*, int*) = NULL;
 const char *ext = strrchr(image_name, '.');
	ext = ext + 1;
	// 파일 이름 문자열 분석
 	// - 확장자가 없으면 처리하지 않음
	if (!ext || ext == image_name)
	{
		return -1;
	}
	// 알맞게 처리 함수를 정하기
	if (strcmp(ext, "jpg") || strcmp(ext, "jpeg"))
	{
		func = jpeg_read;
	}
	if (strcmp(ext, "png"))
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
	}
	if (strcmp(ext, "bmp"))
	{
		func = bmp_read;
	}

 // 확장자에 따른 방법으로 처리 수행 
 // * flags 미구현
 return func(data, image_name, IMAGE_NO_FLAGS);
}
