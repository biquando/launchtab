#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define YY_HEADER_EXPORT_START_CONDITIONS
#include "launchtab.h"
#include "tab.yy.h"

#define DEFAULT_EDITOR "nano"
#define CONFIG ".config/"
#define TABDIR "launchtab/"
#define TAB    "launch.tab"
#define TABPATH CONFIG TABDIR TAB
#define LAUNCHPATH "Library/LaunchAgents"

struct rule *rules;
unsigned int nrules;

const char *caltype[5] = {
	"Minute",
	"Hour",
	"Day",
	"Month",
	"Weekday"
};

const char *fdtype[3] = {
	"In",
	"Out",
	"Err"
};

static void make_dirs(const char *home)
{
	char *path;

	path = try_malloc(strlen(home) + sizeof "/" CONFIG TABDIR);

	strcpy(path, home);

	strcpy(path + strlen(path), "/" CONFIG);
	mkdir(path, 0755);

	strcpy(path + strlen(path), TABDIR);
	mkdir(path, 0755);

	free(path);
}

static FILE *edit_file(const char *file)
{
	char *editor = getenv("EDITOR");
	struct timespec oldtime = {0};
	struct timespec newtime = {0};
	struct stat statbuf;

	if (!editor)
		editor = DEFAULT_EDITOR;

	if (stat(file, &statbuf) == 0)
		oldtime = statbuf.st_mtimespec;

	pid_t child = fork();
	if (child < 0)
		exit(1);
	if (child == 0) {
		execlp(editor, editor, file, NULL);
		exit(2);
	}
	wait(NULL);

	if (stat(file, &statbuf) == 0)
		newtime = statbuf.st_mtimespec;

	if (oldtime.tv_sec == newtime.tv_sec
			&& oldtime.tv_nsec == newtime.tv_nsec) {
		return NULL; /* tab file was not modified */
	}

	return fopen(file, "r");
}

static void invalid_rule(struct rule *r)
{
	fprintf(stderr, "launchtab: invalid rule %s\n", r->id);
}

static void write_plist(char *launchpath, struct rule r)
{
	if (!r.id || !r.command) {
		invalid_rule(&r);
		return;
	}

	char *path = try_malloc(strlen(launchpath) + strlen(r.id) + 2);
	strcpy(path, launchpath);
	strcpy(path + strlen(path), "/");
	strcpy(path + strlen(path), r.id);

	fprintf(stderr, "path: %s\n", path);

	FILE *f = fopen(path, "w");
	if (!f) {
		fprintf(stderr, "launchtab: couldn't open file %s to write\n",
				path);
		return;
	}

	fprintf(f,
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\"\n"
		"  \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
		"<plist version=\"1.0\">\n"
		"<dict>\n"
		"    <key>Label</key>\n"
		"    <string>%s</string>\n", r.id);
	fprintf(f,
		"    <key>ProgramArguments</key>\n"
		"    <array>\n"
		"        <string>/bin/bash</string>\n"
		"        <string>-c</string>\n"
		"        <string>exec %s</string>\n"
		"    </array>\n", r.command);

	/* Interval */
	if (r.interval) {
		fprintf(f,
			"    <key>StartInterval</key>\n"
			"    <integer>%s</integer>\n", r.interval);
	}

	/* Single calendar */
	if (r.ncal == 1) {
		fprintf(f,
			"    <key>StartCalendarInterval</key>\n"
			"    <dict>\n");
		for (int e = 0; e < 5; e++) {
			if (!r.cal->ent[e])
				continue;
			fprintf(f,
				"        <key>%s</key>\n"
				"        <integer>%s</integer>\n",
				caltype[e], r.cal->ent[e]);
		}
		fprintf(f, "    </dict>\n");
	}

	/* Multiple calendars */
	if (r.ncal > 1) {
		fprintf(f,
			"    <key>StartCalendarInterval</key>\n"
			"    <array>\n");
		for (int c = 0; c < r.ncal; c++) {
			fprintf(f, "        <dict>\n");
			for (int e = 0; e < 5; e++) {
				if (!r.cal[c].ent[e])
					continue;
				fprintf(f,
					"            <key>%s</key>\n"
					"            <integer>%s</integer>\n",
					caltype[e], r.cal[c].ent[e]);
			}
			fprintf(f, "        </dict>\n");
		}
		fprintf(f, "    </array>\n");
	}

	/* Environment variables */
	if (r.nvar > 0)
		fprintf(f,
			"    <key>EnvironmentVariables</key>\n"
			"    <dict>\n");
	for (int v = 0; v < r.nvar; v++) {
		fprintf(f,
			"        <key>%s</key>\n"
			"        <string>%s</string>\n",
			r.varlabels[v], r.varvalues[v]);
	}
	if (r.nvar > 0)
		fprintf(f, "    </dict>\n");

	/* File descriptors */
	for (int n = 0; n < 3; n++) {
		if (!r.fd[n])
			continue;
		fprintf(f,
			"    <key>Standard%sPath</key>\n"
			"    <string>%s</string>\n",
			fdtype[n], r.fd[n]);
	}

	if (r.verbatim)
		fprintf(f, "%s", r.verbatim);

	fprintf(f, "</dict>\n"
		"</plist>\n");

	free(path);
	fclose(f);
}

int main(void)
{
	FILE *fd;
	char *home = getenv("HOME");
	char *tabpath;
	char *launchpath;

	if (!home)
		return 5;

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

	/* Test rules */
	printf("\n=================================\n\n");
	for (int i = 0; i < nrules; i++) {
		struct rule r = rules[i];
		printf("[%s]\n", r.id);
		printf("%s\n", r.command);
		if (r.interval)
			printf("Interval: %s\n", r.interval);
		for (int c = 0; c < r.ncal; c++) {
			printf("Calendar:");
			for (int e = 0; e < 5; e++) {
				printf(" %s", r.cal[c].ent[e]);
			}
			printf("\n");
		}
		for (int v = 0; v < r.nvar; v++) {
			printf("Variable: %s = %s\n",
					r.varlabels[v], r.varvalues[v]);
		}
		printf("stdin: %s\n", r.fd[0]);
		printf("stdout: %s\n", r.fd[1]);
		printf("stderr: %s\n", r.fd[2]);
		printf("verbatim: %s\n", r.verbatim);
	}

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
