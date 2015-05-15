// image_buffer.c
#include "image_buffer.h"

void rgb_buffer_calloc(Data *data, int row, int col) {
	int i;

	data->texture.rgb_buffer = (unsigned char **)calloc(row, sizeof(unsigned char *));
	if (!data->texture.rgb_buffer)
		exit(1);
	for (i = 0; i < row; i++)
	{
		data->texture.rgb_buffer[i] = (unsigned char *)calloc(col, sizeof(unsigned char));
		if (!data->texture.rgb_buffer[i])
			exit(1);
	}
}

