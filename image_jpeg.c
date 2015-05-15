// image_jpeg.c - jpeg 포맷을 위한 함수
#include "image_jpeg.h"

// jpeg 포맷 읽기
int jpeg_read(Data *data, char *filename, int flags){
	FILE *jpeg_file;
	unsigned int width, height;
	struct jpeg_decompress_struct jpeg_info;
	struct jpeg_error_mgr jerr;

	jpeg_info.err = jpeg_std_error(&jerr);
	jpeg_file = fopen(filename, "rb");
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

