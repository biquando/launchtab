#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "options.h"
#include "util.h"

#define OPTSTR "dehlr"

static void usage(int err)
{
	fprintf(stderr,
		"usage:  launchtab [ -d ] [ file ]\n"
		"        launchtab [ -d ] { -e | -h | -l | -r }\n"
		"        -d, --debug      show debug messages when installing\n"
		"        -e, --edit       edit user's launchtab\n"
		"        -h, --help       show this message\n"
		"        -l, --list       list user's launchtab\n"
		"        -r, --remove     remove user's launchtab\n"
		);
	exit(err);
}

static void setop(struct taboptions *opts, enum tabop op)
{
	if (opts->op) {
		PRT_E(fprintf(stderr, "only one operation permitted\n"));
		usage(EINVAL);
	}
	opts->op = op;
}

struct taboptions parseopts(int argc, char *argv[])
{
	struct taboptions opts = {0};
	int c;

	static struct option longopts[] = {
		{ "debug",  no_argument, NULL, 'd' },
		{ "edit",   no_argument, NULL, 'e' },
		{ "help",   no_argument, NULL, 'h' },
		{ "list",   no_argument, NULL, 'l' },
		{ "remove", no_argument, NULL, 'r' },
		{ NULL,     0,           NULL, 0 }
	};

	while ((c = getopt_long(argc, argv, OPTSTR, longopts, NULL)) != -1) {
		switch (c) {
		case 'd':
			opts.debug = 1;
			break;
		case 'e':
			setop(&opts, EDTAB);
			break;
		case 'h':
			usage(0);
			break;
		case 'l':
			setop(&opts, LSTAB);
			break;
		case 'r':
			setop(&opts, RMTAB);
			break;
		default:
			usage(EINVAL);
		}
	}
	opts.argc = argc - optind;
	opts.argv = argv + optind;

	if (opts.op && opts.argc > 0) {
		PRT_E(fprintf(stderr, "no arguments permitted after this option\n"));
		usage(EINVAL);
	}

	if (opts.argc > 1) {
		PRT_E(fprintf(stderr, "only one file argument allowed\n"));
		usage(EINVAL);
	}

	return opts;
}
