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
	while (jpeg_info.output_scanline < height) 
	{
//  PDEBUG("jpeg] scanline start at %d\n", jpeg_info.output_scanline);
		jpeg_read_scanlines(&jpeg_info, buffer, 1);
		for(i = 0; i < width; i++) {
			for(j = 0; j < components; j++) {
//    PDEBUG("jpeg] scanline : %d, width : %d, height : %d, counter : %d+%d\n", jpeg_info.output_scanline, width, height, i, j);
				image->pixels[height-jpeg_info.output_scanline][i].l[components - j - 1] = buffer[0][i*components + j];
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

// jpeg 파일 쓰기
int jpeg_write(Image *image, char *filename, int flags) {
 FILE* jpeg_file;
 struct jpeg_compress_struct cinfo;
 struct jpeg_error_mgr jerr;
 JSAMPROW row_pointer[1];
 int row_stride,i,j;

 if(image == NULL || !image) {
  return -1;
 }
 if(!image->pixels || !image->width || !image->height) {
  return -1;
 }

 // alloc & init jpeg_object
 cinfo.err = jpeg_std_error(&jerr);
 jpeg_create_compress(&cinfo);

 // open file
 jpeg_file = fopen(filename, "wb");
 if(jpeg_file == NULL) {
  return -1;
 }
 jpeg_stdio_dest(&cinfo, jpeg_file);

 // set image info for compression
 cinfo.image_width = image->width;
 cinfo.image_height = image->height;
 cinfo.input_components = 3; // use RGB
 cinfo.in_color_space = JCS_RGB; // use RGB

 jpeg_set_defaults(&cinfo);
 jpeg_set_quality(&cinfo, 100, TRUE); // 100 = quality .. fix it to use flags parameter

 // alloc buffer array
 row_stride = image->width * cinfo.input_components;
 row_pointer[0] = (JSAMPROW)malloc(sizeof(JSAMPLE) * row_stride);

 // start compression
 jpeg_start_compress(&cinfo, TRUE);
 while(cinfo.next_scanline < cinfo.image_height) {
  for(i = 0; i < (signed)cinfo.image_width; i++) {
   for(j = 0; j < cinfo.input_components; j++) {
    //PDEBUG("jpeg] scanline : %d, width : %d, height : %d, counter : %d+%d\n", cinfo.next_scanline, cinfo.image_width, cinfo.image_height, i, j);
    row_pointer[0][i*cinfo.input_components + j] = image->pixels[cinfo.image_height - cinfo.next_scanline-1][i].l[cinfo.input_components - j - 1];
   }
  }
  jpeg_write_scanlines(&cinfo, row_pointer, 1);
  //PDEBUG("jpeg_write_scanlines excutes\n");
 }

 jpeg_finish_compress(&cinfo);
 fclose(jpeg_file);
 jpeg_destroy_compress(&cinfo);

 // TODO : 예외처리 추가, 메모리 할당 해제 
 return 0;
}
