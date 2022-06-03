#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "options.h"
#include "style.h"

#define OPTSTR "edlr"

static void usage(int err)
{
	fprintf(stderr, "usage: TODO\n");
	exit(err);
}

static void setop(struct taboptions *opts, enum tabop op)
{
	if (opts->op) {
		fprintf(stderr, LTERR("only one operation permitted\n"));
		usage(EINVAL);
	}
	opts->op = op;
}

struct taboptions parseopts(int argc, char *argv[])
{
	struct taboptions opts = {0};
	int c;

	static struct option longopts[] = {
		{ "edit",   no_argument, NULL, 'e' },
		{ "debug",  no_argument, NULL, 'd' },
		{ "list",   no_argument, NULL, 'l' },
		{ "remove", no_argument, NULL, 'r' },
		{ NULL,     0,           NULL, 0 }
	};

	while ((c = getopt_long(argc, argv, OPTSTR, longopts, NULL)) != -1) {
		switch (c) {
		case 'e':
			setop(&opts, EDTAB);
			break;
		case 'd':
			opts.debug = 1;
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
	argc -= optind;
	argv += optind;

	if (!opts.op)
		opts.op = EDTAB;

	return opts;
}
