// image_jpeg.c - jpeg 포맷을 위한 함수
#include "image_jpeg.h"

// jpeg 포맷 읽기
int jpeg_read(Data *data, char *filename, int flags){
	FILE *jpeg_file;
	unsigned int width, height, components, i;
	struct jpeg_decompress_struct jpeg_info;
	struct jpeg_error_mgr jerr;
	JSAMPARRAY buffer;

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
	components = jpeg_info.output_components;
	// RGBA 는 4바이트
	if(components > 4) components = 4; // 절대 일어나지 않는 일
	
	// rgb값을 모두 저장할 수 있도록 rgb_buffer에 공간 할당
	buffer = (JSAMPARRAY)malloc(sizeof(JSAMPROW) * 1);
	buffer[0] = (JSAMPROW)malloc(sizeof(JSAMPLE) * width * components);
	if(rgb_buffer_calloc(data, height, width))
		return -1;

	while (jpeg_info.output_scanline < height) 
	{
		jpeg_read_scanlines(&jpeg_info, buffer, 1);
		for(i = 0; i < width; i++) {
			for(j = 0; j < components; j++) {
				data->texture.pixels[i][jpeg_info.output_scanline].l[j] = buffer[0][i*components + j];
			}
		}
		// 버그있을듯
	}
	data->texture.width = width;
	data->texture.height = height;
	fclose(jpeg_file);

	jpeg_destroy_decompress(*jpeg_info);
	return 0;
}

