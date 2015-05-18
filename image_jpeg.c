// image_jpeg.c - jpeg 포맷을 위한 함수
#include "image_jpeg.h"

// jpeg 포맷 읽기
int jpeg_read(Image *image, char *filename, int flags){
	FILE *jpeg_file;
	unsigned int width, height, components, i, j;
	struct jpeg_decompress_struct jpeg_info;
	struct jpeg_error_mgr jerr;
	JSAMPARRAY buffer;

	jpeg_info.err = jpeg_std_error(&jerr);
	jpeg_file = fopen(filename, "rb");
	if (jpeg_file == NULL){
		image->pixels = NULL;
		return -1;
	}
	jpeg_create_decompress(&jpeg_info);
	jpeg_stdio_src(&jpeg_info, jpeg_file);
	jpeg_read_header(&jpeg_info, TRUE);
	jpeg_start_decompress(&jpeg_info);
// PDEBUG("jpeg_read phase 1\n");
	width = jpeg_info.output_width;
	height = jpeg_info.output_height;
	components = jpeg_info.output_components;
	// RGBA 는 4바이트
	if(components > 4) components = 4; // 절대 일어나지 않는 일
	
	// rgb값을 모두 저장할 수 있도록 rgb_buffer에 공간 할당
	buffer = (JSAMPARRAY)malloc(sizeof(JSAMPROW) * 1);
	buffer[0] = (JSAMPROW)malloc(sizeof(JSAMPLE) * width * components);
	if(rgb_buffer_calloc(image, width, height))
		return -1;

 image_reset(image);

// PDEBUG("jpeg_read phase 2\n");
	while (jpeg_info.output_scanline < height - 1) 
	{
//  PDEBUG("jpeg] scanline start at %d\n", jpeg_info.output_scanline);
		jpeg_read_scanlines(&jpeg_info, buffer, 1);
		for(i = 0; i < width; i++) {
			for(j = 0; j < components; j++) {
//    PDEBUG("jpeg] scanline : %d, width : %d, height : %d, counter : %d+%d\n", jpeg_info.output_scanline, width, height, i, j);
				image->pixels[jpeg_info.output_scanline][i].l[j] = buffer[0][i*components + j];
			}
		}
//  PDEBUG("jpeg] scanline end at %d\n", jpeg_info.output_scanline);
	}

// PDEBUG("jpeg_read phase 3\n");
 image->width = width;
	image->height = height;
	fclose(jpeg_file);

	jpeg_destroy_decompress(&jpeg_info);
// PDEBUG("jpeg_read end\n");
	return 0;
}

