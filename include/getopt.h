#ifndef GETOPT_H
#define GETOPT_H

// argc option extern
extern int optind;
extern char *optarg;
int getopt(int argc, char *argv[], char *optstring);

#endif