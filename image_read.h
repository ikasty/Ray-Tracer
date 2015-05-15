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

int image_read(Data *data, char *image_name, int flags);

#endif
