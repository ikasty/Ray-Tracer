#ifndef IMAGE_READ_H
#define IMAGE_READ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/jpeglib.h"
#include "include/type.h"

int image_read(Data *data, char *image_name);

#endif
