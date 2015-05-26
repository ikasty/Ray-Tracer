#ifndef IMAGE_JPEG_H
#define IMAGE_JPEG_H

#include "image_buffer.h"
#include "include/images/jpeglib.h"

int jpeg_read(Image *image, char *filename, int flags);
int jpeg_write(Image *image, char *filename, int flags);

#endif
