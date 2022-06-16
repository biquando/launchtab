#ifndef _LAUNCHTAB_STYLE_H
#define _LAUNCHTAB_STYLE_H

#define FBOLD    "\x1b[1m"
#define FITALIC  "\x1b[3m"
#define FUNDER   "\x1b[4m"
#define FRED     "\x1b[31m"
#define FGREEN   "\x1b[32m"
#define FYELLOW  "\x1b[33m"
#define FBLUE    "\x1b[34m"
#define FMAGENTA "\x1b[35m"
#define FCYAN    "\x1b[36m"
#define FRESET   "\x1b[0m"

#define LTWARN(msg) FBOLD "launchtab: " FYELLOW "Warning: " FRESET msg
#define LTWARNL(msg) LTWARN(FBOLD "(line %d) " FRESET) msg
#define LTERR(msg) FBOLD "launchtab: " FRED "Error: " FRESET msg
#define LTERRL(msg) LTERR(FBOLD "(line %d) " FRESET) msg
#define LTINFO(msg) FBOLD "launchtab: " FRESET msg
#define LTINFOL(msg) LTINFO(FBOLD "(line %d) " FRESET) msg

#endif
