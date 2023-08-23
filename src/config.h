#ifndef _LAUNCHTAB_CONFIG_H
#define _LAUNCHTAB_CONFIG_H

#define USAGE_STR                                                              \
"Usage:  launchtab [OPTION]... [FILE]\n"                                       \
"  or:   launchtab [OPTION]... { -c | -e | -h | -l | -R | -r }\n"              \
"  or:   launchtab [OPTION]... -b [FILE]\n"                                    \
"Create or configure a launchtab.\n"                                           \
"\n"                                                                           \
"Operations\n"                                                                 \
"  -b, --backup FILE  backup user's current launchtab to FILE (defaults to \n" \
"                      ~/.config/launchtab/launch.tab.backup)\n"               \
"  -c, --cleanup      clean up temp files\n"                                   \
"  -e, --edit         edit user's launchtab\n"                                 \
"  -h, --help         show this message\n"                                     \
"  -l, --list         list user's launchtab\n"                                 \
"  -R, --reload       reload user's launchtab\n"                               \
"  -r, --remove       remove user's launchtab\n"                               \
"\n"                                                                           \
"Options\n"                                                                    \
"  -d, --debug        show debug messages when installing\n"                   \
"  -q, --quiet        suppress output to stdout/stderr (doesn't affect\n"      \
"                      output from -d)\n"                                      \
""

#define DEFAULT_EDITOR "nano"
#define DEFAULT_SHELL "/bin/sh"
#define DEFAULT_BACKUP "/.config/launchtab/launch.tab.backup"

#define TEMP_TEMPLATE "temp.XXXX"

#endif
