// image_buffer.c
#include "image_buffer.h"

int rgb_buffer_calloc(Data *data, int row, int col) {
	int i;

	data->texture.rgb_buffer = (unsigned char **)calloc(row, sizeof(unsigned char *));
	if (!data->texture.rgb_buffer)
		return -1;
	for (i = 0; i < row; i++)
	{
		data->texture.rgb_buffer[i] = (unsigned char *)calloc(col, sizeof(unsigned char));
		if (!data->texture.rgb_buffer[i])
			return -1;
	}
}

