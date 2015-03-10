



triangle getTriangle(Vertex v[500],Triangle t[500],int id,int num_frame);
hit intersect_triangle(msl_ray ray, triangle triangle);
unsigned int Shading(msl_ray s_ray, triangle s_tri, hit __hit);
void OutputFrameBuffer(int res_x, int res_y, int* FB, char* file_name);
msl_ray gen_ray(camera msl_cam,int current_x,int current_y);
int shadow_test(msl_ray ray, float l_source[3],triangle triangle);