#ifndef _LAUNCHTAB_UTIL_H
#define _LAUNCHTAB_UTIL_H

#include <stdio.h>
#include "style.h"

void *try_malloc(size_t size);
void *try_realloc(void *ptr, size_t size);
extern void free(void *);

char *trim_leading(char *str);
char *trim_trailing(char *str);
char *trim(char *str);
char *str_append(char *str1, char *str2);
char *find_value(char *label, char **labels, char **values, unsigned int n);

int mkdir_p(const char *path);
size_t dirname(const char *path, char *output);
int cpfile(FILE *src, FILE *dst);

extern int debug;
extern int quiet;
#define PRT_D(fn)        if (debug) { fprintf(stderr, "debug: "); fn; }
#define PRT_DL(line, fn) if (debug) { fprintf(stderr, "(%d) debug:", line); fn; }
#define PRT_GEN(str, fn)        if (!quiet) { fprintf(stderr, str);       fn; }
#define PRT_GENL(str, line, fn) if (!quiet) { fprintf(stderr, str, line); fn; }
#define PRT_W(fn)           PRT_GEN(LTWARN(""), fn)
#define PRT_WL(line, fn)    PRT_GENL(LTWARNL(""), line, fn)
#define PRT_E(fn)           PRT_GEN(LTERR(""), fn)
#define PRT_EL(line, fn)    PRT_GENL(LTERRL(""), line, fn)
#define PRT_I(fn)           PRT_GEN(LTINFO(""), fn)
#define PRT_IL(line, fn)    PRT_GENL(LTINFOL(""), line, fn)

#endif
