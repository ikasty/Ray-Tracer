#include <stdlib.h>
#include "glut/glut.h"

#include "images/image_read.h"
#include "do_algorithm.h"
#include "include/type.h"
#include "settings.h"

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

	glFlush();
}

static void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
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

	glutMainLoop();

	return;
}