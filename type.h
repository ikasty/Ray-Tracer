#ifndef TYPE_H
#define TYPE_H

typedef struct
{
	float x,y,z;

} coordinate;

typedef struct  
{
	float max_t;
	float orig[3];
	float dir[3];
	
}msl_ray;

typedef struct
{
	float vert0[3];
	float vert1[3];
	float vert2[3];
	
	
}triangle;
typedef struct
{
	float t,u,v;
	int triangle_id;
}hit;

typedef struct
{
	float orig[3];
	float distance;
	int resx,resy;
}camera;

typedef struct  {
       
       int v1;
       int v2;
       int v3;
       
}Triangle;

typedef struct  {
      
      float x;
      float y;
      float z;
            
}Vertex;

// 전체 데이터를 저장하기 위한 구조체
typedef struct Data {
	Vertex *vert;
	int vert_count;

	Triangle *face;
	int face_count;
} Data;

#endif