#include <stdlib.h>
#include "GL/glut.h"

#include "opengl_menu.h"
#include "opengl_menu_data.h"

static int menu_window;
static int window_width, window_height;

static int current_tab = 0;
static int current_menu = 0, menu_start = 0, menu_end = MENU_COUNT;

static void printString(char text[], int length, int selected)
{
	int i;

	glPushMatrix();
	glColor3f(FONT_COLOR);
	if (selected) glColor3f(NORMAL_COLOR);
	glTranslatef(10.f, 20.f, 0.f);
	
	glRasterPos2i(0, 0);
	glDisable(GL_TEXTURE | GL_TEXTURE_2D);
	for (i = 0; i < length; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, text[i]);
	}
	glEnable(GL_TEXTURE | GL_TEXTURE_2D);

	glPopMatrix();
}

static void draw_menu(void)
{
	int i;

	glutSetWindow(menu_window);

	// 메뉴 창 clear
	glClearColor(MENU_BACKGROUND, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// 좌표를 우리가 알고 있는 윈도우와 일치시킴
	glScalef(1.f, -1.f, 1.f);
	gluOrtho2D(0, window_width, 0, window_height);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// 상단 메뉴 탭 출력
	glPushMatrix();
	for (i = 0; i < TAB_COUNT; i++)
	{
		glColor3f(NORMAL_COLOR);
		if (i == current_tab) glColor3f(SELECTED_COLOR);

		glRecti(MARGIN, MARGIN, TAB_WIDTH - MARGIN, TAB_HEIGHT - MARGIN);

		glTranslatef(TAB_WIDTH, 0, 0);
	}
	glPopMatrix();
	glTranslatef(0, TAB_HEIGHT, 0);

	// 메뉴 내용 출력
	for (i = menu_start; i < menu_end; i++)
	{
		glColor3f(NORMAL_COLOR);
		if (i == current_menu) glColor3f(SELECTED_COLOR);

		glRecti(MARGIN, MARGIN, MENU_WIDTH - MARGIN, MENU_HEIGHT - MARGIN);

		glTranslatef(0, MENU_HEIGHT, 0);
	}

	glFlush();
}

static void change_tab(int diff)
{	if (current_tab + diff >= 0 && current_tab + diff < TAB_COUNT) current_tab += diff;	}
static void change_menu(int diff)
{	if (current_menu + diff >= menu_start && current_menu + diff < menu_end) current_menu += diff;	}

static void keyboard_menu(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // ESC key
		exit(0);
	}
	glutPostRedisplay();
}

static void specialKeyboard_menu(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:		change_menu(-1); break;
	case GLUT_KEY_LEFT:		change_tab (-1); break;
	case GLUT_KEY_DOWN:		change_menu(+1); break;
	case GLUT_KEY_RIGHT:	change_tab (+1); break;
	}
	glutPostRedisplay();
}

static void resize_menu(int width, int height)
{
	glutSetWindow(menu_window);

	window_width = width;
	window_height = height;

	// fix window size
	glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutPostRedisplay();
}

void set_menu(int x_pos, int y_pos)
{
	glutInitWindowPosition(x_pos, y_pos);

	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

	menu_window = glutCreateWindow("Menu");
	glutDisplayFunc(draw_menu);
	glutKeyboardFunc(keyboard_menu);
	glutSpecialFunc(specialKeyboard_menu);
	glutReshapeFunc(resize_menu);
}