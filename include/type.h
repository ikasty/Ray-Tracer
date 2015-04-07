﻿#ifndef TYPE_H
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
	float vert0[3];
	float vert1[3];
	float vert2[3];
	int prim_id;
} Primitive;

typedef struct
{
	float t,u,v;
	int prim_id;

} Hit;

// 전체 데이터를 저장하기 위한 구조체
typedef struct {
	Primitive *primitives;
	int prim_count;

	void *accel_struct;

} Data;

#endif