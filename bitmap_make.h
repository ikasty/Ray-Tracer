


//void OutputFrameBuffer(Shader* SHD, char* file_name);





typedef struct  
{
	unsigned int *FrameBuffer; //�����ӹ���
	int resx, resy; //�ػ�
}Shader;

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef long           LONG;  

typedef struct{
    WORD    bfType;
    DWORD   bfSize;
    WORD    bfReserved1;
    WORD    bfReserved2;
    DWORD   bfOffBits;

} _BITMAPFILEHEADER;

typedef struct{
    DWORD  biSize;
    LONG   biWidth;
    LONG   biHeight;
    WORD   biPlanes;
    WORD   biBitCount;
    DWORD  biCompression;
    DWORD  biSizeImage;
    LONG   biXPelsPerMeter;
    LONG   biYPelsPerMeter;
    DWORD  biClrUsed;
    DWORD  biClrImportant;

} _BITMAPINFOHEADER;
