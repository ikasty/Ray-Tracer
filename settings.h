#ifndef SETTINGS_H
#define SETTINGS_H

#include "include/type.h"

#define MAX_RENDER_DISTANCE		1000000

#define X_SCREEN_SIZE			320
#define Y_SCREEN_SIZE			240
#define CAMERA_DISTANCE			100

#define CAMERA_POS_X			0.0
#define CAMERA_POS_Y			0.0
#define CAMERA_POS_Z			-400.0

#define LIGHT_POS_X				100.0
#define LIGHT_POS_Y				300.0
#define LIGHT_POS_Z				10.0

#define FRAME_COUNT				1

///////////////////////////////////////////////
// option structs
// usage: USE_CAMERA(cam); cam->orig[0];
///////////////////////////////////////////////

struct __screen_info {
	int xsize, ysize;
	int distance, frame_count;
};
extern struct __screen_info __screen__;

#define DEFINE_SCREEN()				\
struct __screen_info __screen__ = {	\
	X_SCREEN_SIZE, Y_SCREEN_SIZE,	\
	CAMERA_DISTANCE, FRAME_COUNT 	\
};
#define USE_SCREEN(__name) struct __screen_info *__name = &__screen__

struct __camera_info {
	float orig[3];
	float distance;
	int resx, resy;
};
extern struct __camera_info __camera__;

#define DEFINE_CAMERA()								\
struct __camera_info __camera__ = {					\
	{CAMERA_POS_X, CAMERA_POS_Y, CAMERA_POS_Z},		\
	CAMERA_DISTANCE,								\
	X_SCREEN_SIZE, Y_SCREEN_SIZE					\
};
#define USE_CAMERA(__name) struct __camera_info *__name = &__camera__

#define DEFINE_LIGHT()								\
float __light__[3] = {								\
	LIGHT_POS_X, LIGHT_POS_Y, LIGHT_POS_Z			\
}
extern float __light__[3];
#define USE_LIGHT(__name) float *__name = __light__

#endif