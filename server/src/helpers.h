#ifndef HELPER_MODULE
#define HELPER_MODULE

#include <stdio.h>
#include <string.h>

#define OK       0
#define NO_INPUT 1
#define TOO_LONG 2

int get_line(char *prmpt, char *buff, size_t sz);

void clearBuffer(char *buf, int charlen);

void print_intro();

#endif /* HELPER_MODULE */