#ifndef IMAGE_GIF_H
#define IMAGE_GIF_H

#include "image_buffer.h"
#include "include/images/gif_lib.h"

int gif_read(Image *image, char *filename, int flags);

#endif
