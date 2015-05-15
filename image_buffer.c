// image_buffer.c
#include "image_buffer.h"

int rgb_buffer_calloc(Data *data, int row, int col) {
	int i;

	data->texture.color = (RGBA **)calloc(col, sizeof(RGBA *));
	if (!data->texture.pixels)
		return -1;
	for (i = 0; i < col; i++)
	{
		data->texture.pixels[i] = (RGBA *)calloc(row, sizeof(RGBA));
		if (!data->texture.pixels[i])
			return -1;
	}
}

