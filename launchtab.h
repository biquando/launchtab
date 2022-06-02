#ifndef _LAUNCHTAB_LAUNCHTAB_H
#define _LAUNCHTAB_LAUNCHTAB_H

void lex_init(void);

void handle_id(void);
void handle_comment(void);
void handle_emptyLine(void);
void handle_commandMulti(void);
void handle_command(void);

void handle_interval(void);
void handle_calendar(void);
void handle_envar(void);
void handle_stdin(void);
void handle_stdout(void);
void handle_stderr(void);

void handle_verbatim(void);
void handle_unknownOpt(void);

void handle_invalid(void);

struct calendar {
	char *ent[5];
};

struct rule {
	char *id;
	char *command;
	char *interval;
	struct calendar *cal;
	unsigned int ncal;
	char **varlabels;
	char **varvalues;
	unsigned int nvar;
	char *fd[3];
	char *verbatim;
};

extern struct rule *rules;
extern unsigned int nrules;

#endif
