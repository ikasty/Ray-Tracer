#ifndef IMAGE_JPEG_H
#define IMAGE_JPEG_H

#include "image_buffer.h"
#include "include/jpeglib.h"

int jpeg_read(Data *data, char *filename, int flags);

#endif
