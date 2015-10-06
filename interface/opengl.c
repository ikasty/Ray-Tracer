#include <stdlib.h>
#include "GL/glut.h"

#include "images/image_read.h"
#include "do_algorithm.h"
#include "include/type.h"
#include "settings.h"

#include "obj_transform.h"

#include "opengl.h"
#include "opengl_menu.h"

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

	glutSetWindow(image_window);
	glutPostRedisplay();
}

static void keyboard(unsigned char key, int x, int y)
{
	glutSetWindow(image_window);
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

static void specialKeyboard(int key, int x, int y)
{
	unsigned char map_key;

	switch (key)
	{
	case GLUT_KEY_UP:		map_key = 'w'; break;
	case GLUT_KEY_LEFT:		map_key = 'a'; break;
	case GLUT_KEY_DOWN:		map_key = 's'; break;
	case GLUT_KEY_RIGHT:	map_key = 'd'; break;
	default:
		map_key = 0;
	}

	keyboard(map_key, x, y);
}

static void resize(int width, int height)
{
	glutSetWindow(image_window);
	window_width = width;
	window_height = height;

	glutPostRedisplay();
}

void do_opengl(Data *data)
{
	USE_SCREEN(screen);
	int screen_xpos, screen_ypos;
	_data = data;

	screen_xpos = (glutGet(GLUT_SCREEN_WIDTH) - screen->xsize) / 2;
	screen_ypos = (glutGet(GLUT_SCREEN_HEIGHT) - screen->ysize) / 2;

	// menu init
	set_menu(screen_xpos + screen->xsize + 100, screen_ypos);

	// screen init
	screen_buffer = image_init(screen->xsize, screen->ysize);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(screen->xsize, screen->ysize);
	glutInitWindowPosition(screen_xpos, screen_ypos);
	image_window = glutCreateWindow("Sojong");
	glutDisplayFunc(draw);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeyboard);
	glutReshapeFunc(resize);

	// gl initialize
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	// transform init
	set_rotate(15.0f);

	glutSetWindow(image_window);
	glutMainLoop();

	return;
}