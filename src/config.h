#ifndef _LAUNCHTAB_CONFIG_H
#define _LAUNCHTAB_CONFIG_H

#define USAGE_STR                                                              \
"Usage:  launchtab [ OPTIONS ] [ file ]\n"                                     \
"        launchtab [ OPTIONS ] { -e | -h | -l | -R | -r }\n"                   \
"\n"                                                                           \
"Operations\n"                                                                 \
"  -e, --edit       edit user's launchtab\n"                                   \
"  -h, --help       show this message\n"                                       \
"  -l, --list       list user's launchtab\n"                                   \
"  -R, --reload     reload user's launchtab\n"                                 \
"  -r, --remove     remove user's launchtab\n"                                 \
"\n"                                                                           \
"Options\n"                                                                    \
"  -d, --debug      show debug messages when installing\n"                     \
"  -q, --quiet      suppress output to stdout/stderr (doesn't affect\n"        \
"                   output from -d)\n"

#define DEFAULT_EDITOR "nano"
#define DEFAULT_SHELL "/bin/sh"

#endif
