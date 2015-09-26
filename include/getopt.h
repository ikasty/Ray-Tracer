#ifndef GETOPT_H
#define GETOPT_H

// argc option extern
extern int optind;
extern char *optarg;
extern char *longopt;
int getopt(int argc, char *argv[], char *optstring);

#define LONG_OPTION(_long_opt, _opt_name) ( (_long_opt) && strncmp((_long_opt), (_opt_name), strlen(_opt_name)) )

#endif