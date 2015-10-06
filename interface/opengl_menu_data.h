#ifndef OPENGL_MENU_DATA
#define OPENGL_MENU_DATA

// 윈도우 가로 길이
#define WINDOW_WIDTH 300

// 상단 탭 크기
#define TAB_HEIGHT 40
#define TAB_WIDTH 120
#define TAB_COUNT 2

// 하단 메뉴 설정
#define MENU_HEIGHT	40
#define MENU_COUNT	5

// 마진 설정
#define MARGIN 2

// 색상 설정
#define FONT_COLOR			0.988f, 0.914f, 0.310f
#define MENU_BACKGROUND		0.204f, 0.396f, 0.643f
#define NORMAL_COLOR		0.204f, 0.396f, 0.643f
#define SELECTED_COLOR		0.769f, 0.627f, 0.000f

// 계산되는 값들
#define WINDOW_HEIGHT	TAB_HEIGHT + MENU_HEIGHT * MENU_COUNT
#define MENU_WIDTH		WINDOW_WIDTH

struct menu_items
{
	int menu_type;
};
static struct menu_tabs
{
	char menu_name[30];
	void(*tabs_func)(void);
	struct menu_items *items;
	int items_count;
} menus[TAB_COUNT];

#endif
