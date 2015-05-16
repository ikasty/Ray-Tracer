#ifndef IMAGE_PNG_H
#define IMAGE_PNG_H

#include "image_buffer.h"
#include "include/png.h"

int png_read(Image *image, char *filename, int flags);

#endif
