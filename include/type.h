#ifndef TYPE_H
#define TYPE_H

typedef unsigned char BYTE;
typedef unsigned short WORD;
#ifdef _WIN32
typedef unsigned long DWORD;
typedef long LONG;
#else
#if defined __UINT32_MAX__ || UINT32_MAX
#include <inttypes.h>
#include <sys/types.h>
#else
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;
#endif
typedef uint32_t DWORD;
typedef int LONG;
#endif

typedef struct
{
	float x,y,z;

} Coordinate;

typedef struct  
{
	float min_t, max_t;
	float orig[3];
	float dir[3];
	
} Ray;

typedef struct
{
	int prim_id;
	float vert[3][3];
	float norm[3][3];
 int use_normal;
	float text[3][3];
	int use_texture;
} Primitive;

typedef struct
{
	float t,u,v;
	int prim_id;

} Hit;

// RGB+A 헤헷
typedef union {
	DWORD i;
	BYTE a,r,g,b;
	BYTE l[4];	
} RGBA;

// 이미지의 속살을 담은 구조체
typedef struct {
	int width;
	int height;
	RGBA **pixels;
} Image;

// 전체 데이터를 저장하기 위한 구조체
typedef struct {
	Primitive *primitives;
	float **norm_on_vertex;
	int prim_count;

	void *accel_struct;
	Image texture;
} Data;
#endif
