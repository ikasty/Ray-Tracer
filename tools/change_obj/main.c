#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	FILE *fp, *wp;
	char buf_orig[100];
	float times = 1;
	float xmove, ymove;

	if (argc == 5)
	{
		char output_file[100];
		fp = fopen(argv[1], "r");

		sprintf(output_file, "%s-re.obj", argv[1]);
		wp = fopen(output_file, "w");

		sscanf(argv[2], "%f", &times);
		sscanf(argv[3], "%f", &xmove);
		sscanf(argv[4], "%f", &ymove);
	}
	else
	{
		printf("usage: change_obj.exe filename times xmove ymove\n");
		return -1;
	}

	while (fgets(buf_orig, 99, fp))
	{
		char *buf = buf_orig;
		int read_size = 0;
		char op[10];

		sscanf(buf, "%s%n", op, &read_size);
		buf += read_size;

		if (strcmp(op, "v") == 0)
		{
			float x, y, z;

			sscanf(buf, "%f %f %f", &x, &y, &z);
			x = x * times + xmove;
			y = y * times + ymove;
			z *= times;
			
			fprintf(wp, "v %f %f %f\n", x, y, z);
		}
		else
		{
			fprintf(wp, "%s", buf_orig);
		}
	}

	return 0;
}