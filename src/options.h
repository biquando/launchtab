#ifndef _LAUNCHTAB_OPTIONS_H
#define _LAUNCHTAB_OPTIONS_H

enum tabop {
	IMTAB=0, EDTAB, LSTAB, RETAB, RMTAB
};

struct taboptions {
	int argc;
	char **argv;
	enum tabop op;
	int debug;
};

struct taboptions parseopts(int argc, char *argv[]);

#endif
