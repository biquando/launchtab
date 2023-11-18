#ifndef _LAUNCHTAB_OPTIONS_H
#define _LAUNCHTAB_OPTIONS_H

enum tabop {
	IMTAB=0, EDTAB, LSTAB, RETAB, RMTAB, CLEAN, BCKUP
};

struct taboptions {
	int argc;
	char **argv;
	enum tabop op;
	int debug;
	int quiet;
    int force;
};

struct taboptions parseopts(int argc, char *argv[]);

#endif
