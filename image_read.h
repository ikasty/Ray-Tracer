#ifndef IMAGE_READ_H
#define IMAGE_READ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/type.h"
#include "image_jpeg.h"
#include "image_png.h"
#include "image_gif.h"
#include "image_tiff.h"
#include "image_bmp.h"

int image_read(Image *image, char *filename, int flags);
int image_write(Image *image, char *filename, int flags); 

#endif
