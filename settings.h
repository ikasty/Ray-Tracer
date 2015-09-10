
/*
Copyright (c) 2015, Minwoo Lee(hellomw@msl.yonsei.ac.kr)
          Daeyoun Kang(mail.ikasty@gmail.com),
                    HyungKwan Park(rpdladps@gmail.com),
                    Ingyu Kim(goracom0@gmail.com),
                    Jungmin Kim(kukakhan@gmail.com)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies, 
either expressed or implied, of the FreeBSD Project.
*/

#ifndef SETTINGS_H
#define SETTINGS_H

#include "include/type.h"

#define DEFAULT_OBJ_FILE		"teapot.obj"
#define DEFAULT_TEXTURE_FILE	"texture_sample.jpg"

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
