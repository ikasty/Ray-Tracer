#include <stdlib.h>
#include "glut/glut.h"

#include "images/image_read.h"
#include "do_algorithm.h"
#include "include/type.h"
#include "settings.h"

#include "obj_transform.h"

#include "opengl.h"

static int image_window, option_window;
static Data *_data;

static void draw(void)
{
	Image *screen_buffer = NULL;
	USE_SCREEN(screen);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	screen_buffer = image_init(screen->xsize, screen->ysize);

	do_algorithm(_data, screen_buffer, NULL);

	// RGBA´Â unsigned byte Å¸ÀÔÀÓ
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
		direction = -2; break;
	case 's':
		direction = -1; break;
	case 'd':
		direction = 2; break;
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

void do_opengl(Data *data)
{
	USE_SCREEN(screen);
	_data = data;

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(screen->xsize, screen->ysize);
	glutInitWindowPosition(100, 100);
	image_window = glutCreateWindow("Sojong");
	glutDisplayFunc(draw);
	glutKeyboardFunc(keyboard);

	// gl initialize
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	// transform init
	set_rotate(15.0f);

	glutMainLoop();

	return;
}