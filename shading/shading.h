#ifndef SHADING_H
#define SHADING_H

#include "include/type.h"

RGBA shading(Ray ray_light_to_point, Primitive primitive, Hit hit, Data *data);

#endif
