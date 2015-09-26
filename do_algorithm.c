#include "include/type.h"
#include "settings.h"
#include "timecheck.h"

#include "gen_ray.h"

// include search and render algorithm
#include "algorithms.h"

// shading algorithm
#include "shading/shading.h"

#include "do_algorithm.h"

void do_algorithm(Data *data, Image *screen_buffer, void (*print_percent)(float percent))
{
	int index_x, index_y; // 스크린의 픽셀별로 통과하는 광선의 x, y축 좌표 인덱스

	USE_SCREEN(screen);
	USE_TIMECHECK();

	// 만약 가속구조체를 사용한다면 빌드함
	if (accel_build)
	{
		// 기존 구조체 해제
		if (clear_accel) (*clear_accel)(data);

		TIMECHECK_START();
		(*accel_build)(data);
		TIMECHECK_END(build_clock);

		PDEBUG("accel_build finished\n");
	}

	// 각 픽셀별로 교차검사를 수행합니다.
	for (index_y = 0; index_y < screen->ysize; index_y++)
	{
		float percent;

		// 진행 상태 출력
		if (print_percent)
		{
			percent = ((float)index_y / screen->ysize) * 100.0f;
			(*print_percent)(percent);
		}

		for (index_x = 0; index_x < screen->xsize; index_x++)
		{
			Ray f_ray = gen_ray((float)index_x, (float)index_y);
			Hit ist_hit;

			// 현재 광선에서 교차검사를 수행함
			TIMECHECK_START();
			ist_hit = (*intersect_search)(data, &f_ray);
			TIMECHECK_END(search_clock);

			// bmp파일을 작성에 필요한 색상정보를 입력합니다.
			if (ist_hit.t > 0)
			{
				//unsigned int *pixel = &screen_buffer[screen->xsize * index_y + index_x];
				RGBA *pixel = &(screen_buffer->pixels[index_y * screen->xsize + index_x]);

				// 교차된 Primitive가 있다면 렌더링함
				TIMECHECK_START();
				*pixel = shading(f_ray, data->primitives[ist_hit.prim_id], ist_hit, data);
				TIMECHECK_END(render_clock);
			}
		} // index_x
	} // index_y
}