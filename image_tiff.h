#ifndef IMAGE_TIFF_H
#define IMAGE_TIFF_H

#include "image_buffer.h"
#include "include/tiffio.h"

int tiff_read(Image *image, char *filename, int flags);

#endif
