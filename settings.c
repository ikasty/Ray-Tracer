#include "settings.h"

//////////////////////////////////
// 각종 옵션들을 위한 구조체 원형들
struct __screen_info __screen__ = {
	X_SCREEN_SIZE, Y_SCREEN_SIZE,
	CAMERA_DISTANCE, FRAME_COUNT
};

struct __camera_info __camera__ = {
	{CAMERA_POS_X, CAMERA_POS_Y, CAMERA_POS_Z},
	CAMERA_DISTANCE,
	X_SCREEN_SIZE, Y_SCREEN_SIZE
};

float __light__[3] = {
	LIGHT_POS_X, LIGHT_POS_Y, LIGHT_POS_Z
};
