#ifndef _LAUNCHTAB_LAUNCHTAB_H
#define _LAUNCHTAB_LAUNCHTAB_H

#include <stdio.h>

#define DEFAULT_EDITOR "nano"
#define CONFIG ".config/"
#define TABDIR "launchtab/"
#define TAB    "launch.tab"
#define TABPATH CONFIG TABDIR TAB
#define LAUNCHPATH "Library/LaunchAgents"
#define DEFAULT_SHELL "/bin/sh"

struct calendar {
	char *ent[5];
};

extern const char *caltype[5];
extern const char *fdtype[3];

struct rule {
	char *id;
	char *command;
	char *interval;
	struct calendar *cal;
	unsigned int ncals;
	char **varlabels;
	char **varvalues;
	unsigned int nvars;
	char *fd[3];
	char *verbatim;
};

struct tab {
	struct rule *rules;
	unsigned int nrules;
	unsigned int ncronrules;

	char **varlabels_glob;
	char **varvalues_glob;
	unsigned int nvars_glob;

	int valid;
};

void lex_tab(FILE *f, struct tab *t);

void handle_cronRule(struct tab *t);
void handle_globEnvar(struct tab *t);
void handle_id(struct tab *t);
void handle_comment(struct tab *t);
void handle_emptyLine(struct tab *t);
void handle_commandMulti(struct tab *t);
void handle_command(struct tab *t);

void handle_interval(struct tab *t);
void handle_calendar(struct tab *t);
void handle_envar(struct tab *t);
void handle_stdin(struct tab *t);
void handle_stdout(struct tab *t);
void handle_stderr(struct tab *t);

void handle_verbatimStart(struct tab *t);
void handle_verbatimLine(struct tab *t);
void handle_verbatimEnd(struct tab *t);
void handle_unknownOpt(struct tab *t);

void handle_invalid(struct tab *t);

extern int debug;
extern int quiet;

#endif
