#ifndef _LAUNCHTAB_OPTIONS_H
#define _LAUNCHTAB_OPTIONS_H

enum tabop {
	EDTAB=1, LSTAB, RMTAB
};

struct taboptions {
	enum tabop op;
	int debug;
};

struct taboptions parseopts(int argc, char *argv[]);

#endif
