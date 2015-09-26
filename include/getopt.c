#include <string.h>
#include "getopt.h"

//////////////////////////////////////////////////////////////////////////
// implements getopt(), a function to parse command lines.
// original author: Hans Dietrich (hdietrich2@hotmail.com) at XGetopt.cpp
// modified for C by Daeyoun Kang

int optind = 0;
char *optarg;
char *longopt;

int getopt(int argc, char *argv[], char *optstring)
{
	static char *next = NULL;
	char c, *cp;

	optarg = NULL;
	longopt = NULL;

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
		// long option
		longopt = next++;

		// search value (if exist)
		for (; *next && (*next) != '='; next++);
		if (*next)
		{
			optarg = next;
			next = NULL;
		}
		else if (optind < argc)
		{
			optarg = argv[optind];
			optind++;
		}
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