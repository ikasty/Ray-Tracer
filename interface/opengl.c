#include <stdlib.h>
#include "glut/glut.h"

#include "images/image_read.h"
#include "do_algorithm.h"
#include "include/type.h"
#include "settings.h"

#include "obj_transform.h"

#include "opengl.h"

static int image_window;
static int window_width, window_height;
static Data *_data;
static Image *screen_buffer = NULL;

static void draw(void)
{
	USE_SCREEN(screen);

	glutSetWindow(image_window);

	glClear(GL_COLOR_BUFFER_BIT);
	image_reset(screen_buffer);

	// 이미지 연산
	do_algorithm(_data, screen_buffer, NULL);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// bitmap은 unsigned byte로 4byte(R, G, B, A순서) 형식임
	glDrawPixels(screen->xsize, screen->ysize, GL_RGBA, GL_UNSIGNED_BYTE, screen_buffer->pixels);

	glFlush();
}

static void rotateObj(unsigned char key)
{
	int i, j;
	int direction = 0;

	switch (key)
	{
	case 'w':
		direction = 1; break;
	case 'a':
		direction = 2; break;
	case 's':
		direction = -1; break;
	case 'd':
		direction = -2; break;
	}

	for (i = 0; i < _data->prim_count; i++)
	{
		for (j = 0; j < 3; j++)
		{
			get_rotated_vector(_data->primitives[i].vert[j], direction);
			get_rotated_vector(_data->primitives[i].norm[j], direction);
		}
	}

	glutPostRedisplay();
}

static void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
	case 'a':
	case 's':
	case 'd':
		rotateObj(key);
		break;

	case 27: // ESC key
		exit(0);

	default:
		break;
	}
}

static void resize(int width, int height)
{
	window_width = width;
	window_height = height;

	glutPostRedisplay();
}

void do_opengl(Data *data)
{
	USE_SCREEN(screen);
	int screen_width, screen_height;
	_data = data;

	screen_width = glutGet(GLUT_SCREEN_WIDTH);
	screen_height = glutGet(GLUT_SCREEN_HEIGHT);

	// screen init
	screen_buffer = image_init(screen->xsize, screen->ysize);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(screen->xsize, screen->ysize);
	glutInitWindowPosition((screen_width - screen->xsize) / 2, (screen_height - screen->ysize) / 2);
	image_window = glutCreateWindow("Sojong");
	glutDisplayFunc(draw);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(resize);

	// gl initialize
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	// transform init
	set_rotate(15.0f);

	glutMainLoop();

	return;
}