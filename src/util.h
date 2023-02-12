#ifndef _LAUNCHTAB_UTIL_H
#define _LAUNCHTAB_UTIL_H

#include <stdio.h>

void *try_malloc(size_t size);
void *try_realloc(void *ptr, size_t size);
extern void free(void *);

char *trim_leading(char *str);
char *trim_trailing(char *str);
char *trim(char *str);
char *str_append(char *str1, char *str2);
char *find_value(char *label, char **labels, char **values, unsigned int n);
char *escape_xml(char *str);

int mkdir_p(const char *path);
size_t dirname(const char *path, char *output);
int cpfile(FILE *src, FILE *dst);

void print_dbg(char *format, ...);
void print_warn(char *format, ...);
void print_warnl(char *format, unsigned lineno, ...);
void print_err(char *format, ...);
void print_errl(char *format, unsigned lineno, ...);
void print_info(char *format, ...);
void print_infol(char *format, unsigned lineno, ...);

#endif
