#ifndef IMAGE_BUFFER_H
#define IMAGE_BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/type.h"
#include "image_flags.h"
#include "include/debug-msg.h"

int rgb_buffer_calloc(Image *image, int row, int col);
Image* image_init(unsigned int width, unsigned int height);
int image_free(Image* image);
int image_reset(Image* image);

#endif
