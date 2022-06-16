#ifndef _LAUNCHTAB_WRITER_H
#define _LAUNCHTAB_WRITER_H

#include <stdio.h>
#include "launchtab.h"

int edit_file(const char *file);
void write_plist(char *launchpath, struct rule r);

#endif
