#ifndef _LAUNCHTAB_WRITER_H
#define _LAUNCHTAB_WRITER_H

#include <stdio.h>
#include "launchtab.h"

int edit_file(char *path);
void write_plist(char *path, struct tab *t, struct rule *r);
int rm_temps(char *dir, char *tmpname);

struct tab read_tab(char *path);
void free_tab(struct tab *t);
void debug_tab(struct tab *t);
void uninstall_tab(char *launchpath, struct tab *t);
void install_tab(char *launchpath, struct tab *t);

#endif
