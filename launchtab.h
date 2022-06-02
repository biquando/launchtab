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

void handle_verbatimStart(void);
void handle_verbatimEnd(void);
void handle_verbatimLine(void);
void handle_unknownOpt(void);

struct calendar {
	unsigned char minute;
	unsigned char hour;
	unsigned char day;
	unsigned char month;
	unsigned char weekday;
};

struct rule {
	char *id;
	char *command;
	unsigned long interval;
	struct calendar *cal;
	unsigned int ncal;
	char *fd[3];
	char *verbatim;
};

void rule_init(struct rule *r);
void rule_free(struct rule *r);

#endif
