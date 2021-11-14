#ifndef UTILS
#define UTILS

#include <stdio.h> /*   fprintf     */
#include <fcntl.h>

#define	EXIT_FAILURE	1   // Instead of including the entirety of stdlib...
#define FAIL_SUCCESFULLY(msg)       { fprintf(stderr, msg); exit(EXIT_FAILURE); }
#define FAIL_FSUCCESFULLY(msg, ...) { fprintf(stderr, msg, __VA_ARGS__); exit(EXIT_FAILURE); }

char* getHelpMessage();

int isnumber(char* potential);  // ctype->isnumber only defined in BSD

char* lockToString(struct flock *fl, char *dest);

#endif