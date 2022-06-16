#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "launchtab.h"
#include "options.h"
#include "style.h"
#include "tab.yy.h"
#include "writer.h"
#include "util.h"

struct rule *rules;
unsigned int nrules;
unsigned int ncronrules;
char **varlabels_glob;
char **varvalues_glob;
unsigned int nvars_glob;
int debug;
int quiet = 0;

static char *tabpath;     /*  ~/.config/launchtab/launch.tab  */
static char *launchpath;  /*  ~/Library/LaunchAgents          */

static void install_tab()
{
	FILE *fd = fopen(tabpath, "r");
	if (!fd) {
		perror(NULL);
		exit(errno);
	}

	/* Initialize rules */
	rules = NULL;
	nrules = 0;
	ncronrules = 0;

	/* Initialize global envars */
	varlabels_glob = NULL;
	varvalues_glob = NULL;
	nvars_glob = 0;

	yyin = fd;
	lex_init();
	yylex();

	/* Print rules */
	if (debug) {
		fprintf(stderr, "\n=================================\n\n");
		for (int i = 0; i < nrules; i++) {
			struct rule r = rules[i];
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

	/* Write rules */
	for (int r = 0; r < nrules; r++) {
		write_plist(launchpath, rules[r]);
	}

	/* Free rules */
	for (int i = 0; i < nrules; i++) {
		struct rule r = rules[i];
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
	free(rules);

	/* Free global envars */
	for (int i = 0; i < nvars_glob; i++) {
		free(varlabels_glob[i]);
		free(varvalues_glob[i]);
	}
	free(varlabels_glob);
	free(varvalues_glob);

	fclose(fd);

}

static void import_tab(FILE *fd)
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

	install_tab();
}

static void edit_tab()
{
	if (!edit_file(tabpath)) {
		print_info("no changes made to "TAB"\n");
		return;
	}
	install_tab();
}

static void list_tab()
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

static void remove_tab()
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

	tabpath = try_malloc(strlen(home) + sizeof "/" TABPATH);
	strcpy(tabpath, home);
	strcpy(tabpath + strlen(home), "/" TABPATH);

	launchpath = try_malloc(strlen(home) + sizeof "/" LAUNCHPATH);
	strcpy(launchpath, home);
	strcpy(launchpath + strlen(home), "/" LAUNCHPATH);

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
		import_tab(fd);
		break;
	case EDTAB:
		edit_tab();
		break;
	case LSTAB:
		list_tab();
		break;
	case RMTAB:
		remove_tab();
		break;
	default:
		exit(EINVAL);
	}

	free(tabpath);
	free(launchpath);
	return 0;
}
