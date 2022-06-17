#ifndef _LAUNCHTAB_WRITER_H
#define _LAUNCHTAB_WRITER_H

#include <stdio.h>
#include "launchtab.h"

int edit_file(const char *file);
void write_plist(char *path, struct tab *t, struct rule *r);

void uninstall_tab(char *launchpath, struct tab *t);
void install_tab(char *launchpath, struct tab *t);

#endif
