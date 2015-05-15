#ifndef IMAGE_GIF_H
#define IMAGE_GIF_H

#include "image_buffer.h"
#include "include/gif_lib.h"

int gif_read(Data *data, char *filename, int flags);

#endif
