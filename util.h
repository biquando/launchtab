#ifndef _LAUNCHTAB_UTIL_H
#define _LAUNCHTAB_UTIL_H

#include <stdio.h>

void *try_malloc(size_t size);
void *try_realloc(void *ptr, size_t size);

const char *trim_leading(const char *str);
char *trim_trailing(char *str);
char *trim(char *str);

int mkdir_p(const char *path);
size_t dirname(const char *path, char *output);
int cpfile(FILE *src, FILE *dst);

void print_dbg(char *format, ...);

#endif
