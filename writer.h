#ifndef _LAUNCHTAB_WRITER_H
#define _LAUNCHTAB_WRITER_H

#include <stdio.h>
#include "launchtab.h"

int mkdir_p(const char *path);
size_t dirname(const char *path, char *output);

FILE *edit_file(const char *file);
void write_plist(char *launchpath, struct rule r);

void print_dbg(char *format, ...);

#endif
