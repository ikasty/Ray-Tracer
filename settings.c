#include <string.h>

#include "settings.h"

int optind = 0;
char *optarg;

int getopt(int argc, char *argv[], char *optstring)
{
	static char *next = NULL;
	char c, *cp;

	optarg = NULL;

	if (next == NULL || *next == '\0')
	{
		if (!optind) optind++;

		if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')
		{
			optarg = NULL;
			if (optind < argc) optarg = argv[optind];
			return -1;
		}

		next = argv[optind];
		next++;
		optind++;
	}

	c = *next++;
	cp = strchr(optstring, c);

	if (c == '-')
	{
		optarg = next;
		return '-';
	}
	if (cp == NULL || c == ':')
		return '?';
	
	cp++;
	if (*cp == ':')
	{
		if (*next != '\0')
		{
			optarg = next;
			next = NULL;
		}
		else if (optind < argc)
		{
			optarg = argv[optind];
			optind++;
		}
		else
		{
			return '?';
		}
	}

	return c;
}