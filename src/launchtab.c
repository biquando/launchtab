#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "launchtab.h"
#include "options.h"
#include "style.h"
#include "util.h"
#include "writer.h"

int debug;
int quiet = 0;
static char *tabpath;     /*  ~/.config/launchtab/launch.tab  */
static char *launchpath;  /*  ~/Library/LaunchAgents          */

static void _install_tab()
{
	FILE *tabf = fopen(tabpath, "r");
	if (!tabf) {
		perror(NULL);
		exit(errno);
	}

	struct tab t = {0};
	lex_tab(tabf, &t);
	fclose(tabf);

	/* Print rules */
	if (debug) {
		fprintf(stderr, "\n=================================\n\n");
		for (int i = 0; i < t.nrules; i++) {
			struct rule r = t.rules[i];
			fprintf(stderr, "[%s]\n", r.id);
			fprintf(stderr, "%s\n", r.command);
			if (r.interval)
				fprintf(stderr, "Interval: %s\n", r.interval);
			for (int c = 0; c < r.ncals; c++) {
				fprintf(stderr, "Calendar:");
				for (int e = 0; e < 5; e++) {
					fprintf(stderr, " %s", r.cal[c].ent[e]);
				}
				fprintf(stderr, "\n");
			}
			for (int v = 0; v < r.nvars; v++) {
				fprintf(stderr, "Variable: %s = %s\n",
						r.varlabels[v], r.varvalues[v]);
			}
			fprintf(stderr, "stdin: %s\n", r.fd[0]);
			fprintf(stderr, "stdout: %s\n", r.fd[1]);
			fprintf(stderr, "stderr: %s\n", r.fd[2]);
			fprintf(stderr, "verbatim: %s\n", r.verbatim);
		}
	}

	install_tab(launchpath, &t);

	/* Free rules */
	for (int i = 0; i < t.nrules; i++) {
		struct rule r = t.rules[i];
		free(r.id);
		free(r.command);
		free(r.cal);
		for (int v = 0; v < r.nvars; v++) {
			free(r.varlabels[v]);
			free(r.varvalues[v]);
		}
		free(r.varlabels);
		free(r.varvalues);
		free(r.fd[0]);
		free(r.fd[1]);
		free(r.fd[2]);
		free(r.verbatim);
	}
	free(t.rules);

	/* Free global envars */
	for (int i = 0; i < t.nvars_glob; i++) {
		free(t.varlabels_glob[i]);
		free(t.varvalues_glob[i]);
	}
	free(t.varlabels_glob);
	free(t.varvalues_glob);
}

static void _import_tab(FILE *fd)
{
	if (!fd) {
		if (!(fd = tmpfile())) {
			perror(NULL);
			exit(errno);
		}
		if (cpfile(stdin, fd) < 0) {
			perror(NULL);
			exit(errno);
		}
	}

	FILE *tabfd;
	if (!(tabfd = fopen(tabpath, "w"))) {
		perror(NULL);
		exit(errno);
	}
	if (cpfile(fd, tabfd) < 0) {
		perror(NULL);
		exit(errno);
	}

	fclose(fd); /* note that this function closes fd */
	fclose(tabfd);

	_install_tab();
}

static void _edit_tab()
{
	if (!edit_file(tabpath)) {
		print_info("no changes made\n");
		return;
	}
	_install_tab();
}

static void _list_tab()
{
	FILE *fd = fopen(tabpath, "r");
	if (!fd && errno == ENOENT) {
		if (errno == ENOENT)
			print_err("user does not have a launchtab\n");
		else
			perror(NULL);
		exit(errno);
	}

	if (cpfile(fd, stdout) < 0) {
		perror(NULL);
		exit(errno);
	}

	fclose(fd);
}

static void _remove_tab()
{
	if (unlink(tabpath) < 0) {
		perror(NULL);
		exit(errno);
	}
}

int main(int argc, char *argv[])
{
	char *home = getenv("HOME");

	if (!home) {
		print_err("missing $HOME variable.\n");
		exit(ENOENT);
	}

	tabpath = str_append(NULL, home);
	tabpath = str_append(tabpath, "/.config/launchtab/launch.tab");

	launchpath = str_append(NULL, home);
	launchpath = str_append(launchpath, "/Library/LaunchAgents");

	/* mkdir_p: dirname(tabpath) and launchpath */
	size_t tabdirlen = dirname(tabpath, NULL);
	char tmpc = tabpath[tabdirlen];
	tabpath[tabdirlen] = '\0';
	if (mkdir_p(launchpath) < 0) {
		print_err("couldn't create directory: %s\n", launchpath);
		perror(NULL);
		exit(errno);
	}
	if (mkdir_p(tabpath) < 0) {
		print_err("couldn't create directory: %s\n", tabpath);
		perror(NULL);
		exit(errno);
	}
	tabpath[tabdirlen] = tmpc;

	struct taboptions opts = parseopts(argc, argv);
	argc = opts.argc;
	argv = opts.argv;
	debug = opts.debug;

	FILE *fd;
	switch (opts.op) {
	case IMTAB:
		if (argc > 0) {
			fd = fopen(argv[0], "r");
			if (!fd) {
				print_err("%s: ", argv[0]);
				perror(NULL);
				exit(errno);
			}
		} else {
			fd = NULL;
		}
		_import_tab(fd);
		break;
	case EDTAB:
		_edit_tab();
		break;
	case LSTAB:
		_list_tab();
		break;
	case RMTAB:
		_remove_tab();
		break;
	default:
		exit(EINVAL);
	}

	free(tabpath);
	free(launchpath);
	return 0;
}
