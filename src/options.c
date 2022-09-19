#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "options.h"
#include "util.h"

#define OPTSTR "bcdehlqRr"

static void usage(int err)
{
	fprintf(stderr, USAGE_STR);
	exit(err);
}

static void setop(struct taboptions *opts, enum tabop op)
{
	if (opts->op) {
		print_err("only one operation permitted\n");
		usage(EINVAL);
	}
	opts->op = op;
}

struct taboptions parseopts(int argc, char *argv[])
{
	struct taboptions opts = {0};
	int c;

	static struct option longopts[] = {
		{ "backup",  no_argument, NULL, 'b' },
		{ "cleanup", no_argument, NULL, 'c' },
		{ "debug",   no_argument, NULL, 'd' },
		{ "edit",    no_argument, NULL, 'e' },
		{ "help",    no_argument, NULL, 'h' },
		{ "list",    no_argument, NULL, 'l' },
		{ "quiet",   no_argument, NULL, 'q' },
		{ "reload",  no_argument, NULL, 'R' },
		{ "remove",  no_argument, NULL, 'r' },
		{ NULL,      0,           NULL, 0 }
	};

	while ((c = getopt_long(argc, argv, OPTSTR, longopts, NULL)) != -1) {
		switch (c) {
		case 'b':
			setop(&opts, BCKUP);
			break;
		case 'c':
			setop(&opts, CLEAN);
			break;
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
		case 'q':
			opts.quiet = 1;
			break;
		case 'R':
			setop(&opts, RETAB);
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
		print_err("no arguments permitted after this option\n");
		usage(EINVAL);
	}

	if (opts.argc > 1) {
		print_err("only one file argument allowed\n");
		usage(EINVAL);
	}

	return opts;
}
