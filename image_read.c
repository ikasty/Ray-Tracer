// image_read.c
#include "image_read.h"


void rgb_buffer_calloc(Data *data, int row, int col)
{
	int i;

	data->texture.rgb_buffer = (unsigned char **)calloc(row, sizeof(unsigned char *));
	if (!data->texture.rgb_buffer)
		exit(1);
	for (i = 0; i < row; i++)
	{
		data->texture.rgb_buffer[i] = (unsigned char *)calloc(col, sizeof(unsigned char));
		if (!data->texture.rgb_buffer[i])
			exit(1);
	}
}

static int jpeg_read(Data *data, char *jpeg_name){
	FILE *jpeg_file;
	unsigned int width, height;
	struct jpeg_decompress_struct jpeg_info;
	struct jpeg_error_mgr jerr;

	jpeg_info.err = jpeg_std_error(&jerr);
	jpeg_file = fopen(jpeg_name, "rb");
	if (jpeg_file == NULL){
		data->texture.rgb_buffer = NULL;
		return -1;
	}
	jpeg_create_decompress(&jpeg_info);
	jpeg_stdio_src(&jpeg_info, jpeg_file);
	jpeg_read_header(&jpeg_info, TRUE);
	jpeg_start_decompress(&jpeg_info);

	width = jpeg_info.output_width;
	height = jpeg_info.output_height;
	
	
	// rgb값을 모두 저장할 수 있도록 rgb_buffer에 공간 할당
	rgb_buffer_calloc(data, height, width * 3);
	while (jpeg_info.output_scanline < height) 
	{
		jpeg_read_scanlines(&jpeg_info, &data->texture.rgb_buffer[jpeg_info.output_scanline], 1);
	}
	data->texture.width = width;
	data->texture.height = height;
	fclose(jpeg_file);

 return 0;
}

int image_read(Data *data, char *image_name){
 int (*func)(Data*, char*)i = NULL;
 const char *ext = strrchr(image_name, '.');
	ext = ext + 1;
	// 파일 이름 문자열 분석
 // - 확장자가 없으면 처리하지 않음
	if (!ext || ext == image_name)
	{
		return;
	}
 // - *.jpg, *.jpeg
	if (strcmp(ext, "jpg") || strcmp(ext, "jpeg"))
	{
		func = jpeg_read;
	}

 // 확장자에 따른 방법으로 처리 수행 
 return func(data, image_name);
}
