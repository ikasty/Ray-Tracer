﻿// image_buffer.c
#include "image_buffer.h"

int rgb_buffer_calloc(Image *image, int col, int row) {
	image->pixels = (RGBA *)calloc(row * col, sizeof(RGBA));
	if (!image->pixels)
		return -1;
	
	return 0;
}

Image* image_init(unsigned int width, unsigned int height) {

 Image* image = NULL;

 //PDEBUG("image init start\n");
 image = (Image*)malloc(sizeof(Image));
 image->width = width;
 image->height = height;

 if(!image)
  return NULL;

 if(rgb_buffer_calloc(image, width, height)) {
  image_free(image);
  return NULL;
 }

 //PDEBUG("image init end\n");
 return image;
}

int image_free(Image* image) {
 // 미구현
 return -1;
}

int image_reset(Image* image) {
 //PDEBUG("image reset start\n");
 memset(image->pixels, 0, sizeof(RGBA) * image->width * image->height);

 //PDEBUG("image reset end\n");
 return 0;
}
