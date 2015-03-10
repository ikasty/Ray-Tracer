typedef struct
{
	float x,y,z;

} coordinate;

typedef struct  
{
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