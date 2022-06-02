#ifndef _LAUNCHTAB_WRITER_H
#define _LAUNCHTAB_WRITER_H

#include <stdio.h>
#include "launchtab.h"

void make_dirs(const char *home);
FILE *edit_file(const char *file);
void write_plist(char *launchpath, struct rule r);

#endif
