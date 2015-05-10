#ifndef SETTINGS_H
#define SETTINGS_H

#include "include/type.h"

#define DEFAULT_OBJ_FILE		"teapot.obj"
#define DEFAULT_TEXTURE_FILE	"tex_0009_1.jpg"

#define MIN_RENDER_DISTANCE		0
#define MAX_RENDER_DISTANCE		1000000

#define X_SCREEN_SIZE			320
#define Y_SCREEN_SIZE			240
#define CAMERA_DISTANCE			100

#define CAMERA_POS_X			0.0
#define CAMERA_POS_Y			0.0
#define CAMERA_POS_Z			-400.0

#define LIGHT_POS_X				100.0
#define LIGHT_POS_Y				100.0
#define LIGHT_POS_Z				-100.0

#define FRAME_COUNT				10

#define SHINESS					300

///////////////////////////////////////////////
// option structs
// usage: USE_CAMERA(cam); cam->orig[0];
///////////////////////////////////////////////
#define USE_SCREEN(__name) struct __screen_info *__name = &__screen__
#define USE_CAMERA(__name) struct __camera_info *__name = &__camera__
#define USE_LIGHT(__name) float *__name = __light__

struct __screen_info {
	int xsize, ysize;
	int distance, frame_count;
};
extern struct __screen_info __screen__;

struct __camera_info {
	float orig[3];
	float distance;
};
extern struct __camera_info __camera__;

extern float __light__[3];

#endif