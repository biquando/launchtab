#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YY_HEADER_EXPORT_START_CONDITIONS
#include "launchtab.h"
#include "tab.yy.h"
#include "writer.h"

struct rule *rules;
unsigned int nrules;
int debug = 0;

int main(void)
{
	FILE *fd;
	char *home = getenv("HOME");
	char *tabpath;
	char *launchpath;

	if (!home) {
		fprintf(stderr, "Error: Missing $HOME variable.\n");
		exit(ENOENT);
	}

	tabpath = try_malloc(strlen(home) + sizeof "/" TABPATH);
	strcpy(tabpath, home);
	strcpy(tabpath + strlen(home), "/" TABPATH);

	launchpath = try_malloc(strlen(home) + sizeof "/" LAUNCHPATH);
	strcpy(launchpath, home);
	strcpy(launchpath + strlen(home), "/" LAUNCHPATH);

	make_dirs(home);
	fd = edit_file(tabpath);
	if (!fd) {
		fprintf(stderr, "launchtab: no changes made to "TAB"\n");
		return 0;
	}

	/* Initialize rules */
	rules = NULL;
	nrules = 0;

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
			for (int c = 0; c < r.ncal; c++) {
				fprintf(stderr, "Calendar:");
				for (int e = 0; e < 5; e++) {
					fprintf(stderr, " %s", r.cal[c].ent[e]);
				}
				fprintf(stderr, "\n");
			}
			for (int v = 0; v < r.nvar; v++) {
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
		for (int v = 0; v < r.nvar; v++) {
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

	fclose(fd);
	free(tabpath);
	return 0;
}
