#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "launchtab.h"
#include "style.h"
#include "util.h"

extern char *yytext;
extern int yylineno;

static struct rule *new_rule(void)
{
	nrules++;
	rules = try_realloc(rules, nrules * sizeof *rules);

	struct rule *r = &rules[nrules - 1];
	memset(r, 0, sizeof *r);
	return r;
}

static char *add_calendar(struct rule *r)
{
	r->ncals++;
	r->cal = try_realloc(r->cal, r->ncals * sizeof *r->cal);
	struct calendar *c = &r->cal[r->ncals - 1];
	memset(c, 0, sizeof *c);

	int i = -1;
	char *entry;
	for (entry = strtok(yytext, " \t");
	     entry && i < 4;
	     entry = strtok(NULL, " \t")) {
		i++;
		if (entry[0] == '*')
			continue;
		c->ent[i] = try_malloc(strlen(entry) + 1);
		strcpy(c->ent[i], entry);
	}
	return entry;
}

static char *parse_value(char *value)
{
	value = trim(value);
	int len = strlen(value);

	if (len > 1 && (value[0] == '\'' || value[len-1] == '\"')
			&& value[0] == value[len-1]) {
		value[len-1] = '\0';
		value++;
	}

	return value;
}

static void parse_envar(char *text, char ***labs_p, char ***vals_p,
		unsigned int *nvar_p)
{
	char *label = text;
	char *value = text;
	while (*value != '=') value++;
	*value = '\0';
	value++;

	label = trim(label);
	value = parse_value(value);

	int label_len = strlen(label);
	int value_len = strlen(value);

	(*nvar_p)++;
	*labs_p = try_realloc(*labs_p, *nvar_p * sizeof **labs_p);
	*vals_p = try_realloc(*vals_p, *nvar_p * sizeof **vals_p);

	(*labs_p)[*nvar_p - 1] = try_malloc(label_len);
	(*vals_p)[*nvar_p - 1] = try_malloc(value_len);
	strcpy((*labs_p)[*nvar_p - 1], label);
	strcpy((*vals_p)[*nvar_p - 1], value);
}


void handle_cronRule(void)
{
	print_dbg("cronRule: %s", yytext);
	yytext = trim(yytext);

	struct rule *r = new_rule();
	r->id = try_malloc(sizeof "cron.rule." + ncronrules/10);
	sprintf(r->id, "cron.rule.%u", ncronrules++);

	yytext = add_calendar(r);
	if (yytext) {
		/* The token should end with a newline */
		for (int i = 0; yytext[i] != '\n'; i++) {
			if (yytext[i] == '\0') {
				yytext[i] = ' ';
				break;
			}
		}

		yytext = trim(yytext);
		r->command = try_malloc(strlen(yytext) + 1);
		strcpy(r->command, yytext);
	}
}

void handle_globEnvar(void)
{
	print_dbg("globEnvar: %s", yytext);
	parse_envar(yytext, &varlabels_glob, &varvalues_glob, &nvars_glob);
}

void handle_id(void)
{
	print_dbg("id: %s", yytext);
	yytext = trim(yytext);

	struct rule *r = new_rule();

	int len = strlen(yytext);
	yytext[len - 1] = '\0'; /* remove trailing ] */
	r->id = try_malloc(len);
	strcpy(r->id, yytext + 1); /* remove leading [ */
}

void handle_comment(void)
{
	print_dbg("comment: %s", yytext);
}

void handle_emptyLine(void)
{
	print_dbg("emptyLine: %s", yytext);
}

void handle_commandMulti(void)
{
	handle_command();
}

void handle_command(void)
{
	print_dbg("command: %s", yytext);
	yytext = trim(yytext);
	int len = strlen(yytext);
	if (len > 1 && yytext[len-1] == '\\') {
		yytext[len-1] = '\0'; /* remove trailing backslash */
	}

	struct rule *r = &rules[nrules - 1];
	r->command = str_append(r->command, yytext);
}


/* Options */

void handle_interval(void)
{
	print_dbg("interval: %s", yytext);
	yytext = trim(yytext);
	yytext += sizeof("Interval");
	yytext = trim(yytext);

	struct rule *r = &rules[nrules - 1];
	r->interval = try_realloc(r->interval, strlen(yytext) + 1);
	strcpy(r->interval, yytext);
}

void handle_calendar(void)
{
	print_dbg("calendar: %s", yytext);
	yytext = trim(yytext);

	add_calendar(&rules[nrules - 1]);
}

void handle_envar(void)
{
	print_dbg("envar: %s", yytext);
	struct rule *r = &rules[nrules - 1];
	parse_envar(yytext, &r->varlabels, &r->varvalues, &r->nvars);
}

void handle_stdin(void)
{
	print_dbg("stdin: %s", yytext);
	yytext = trim(yytext);
	yytext += sizeof "<" - 1;
	yytext = trim(yytext);

	struct rule *r = &rules[nrules - 1];
	r->fd[0] = try_realloc(r->fd[0], strlen(yytext) + 1);
	strcpy(r->fd[0], yytext);
}

void handle_stdout(void)
{
	print_dbg("stdout: %s", yytext);
	yytext = trim(yytext);
	yytext += sizeof ">" - 1;
	yytext = trim(yytext);

	struct rule *r = &rules[nrules - 1];
	r->fd[1] = try_realloc(r->fd[1], strlen(yytext) + 1);
	strcpy(r->fd[1], yytext);
}

void handle_stderr(void)
{
	print_dbg("stderr: %s", yytext);
	yytext = trim(yytext);
	yytext += sizeof "2>" - 1;
	yytext = trim(yytext);

	struct rule *r = &rules[nrules - 1];
	r->fd[2] = try_realloc(r->fd[2], strlen(yytext) + 1);
	strcpy(r->fd[2], yytext);
}


void handle_verbatimStart(void)
{
	print_dbg("verbatimStart: %s", yytext);
}

void handle_verbatimLine(void)
{
	print_dbg("verbatimLine: %s", yytext);
	struct rule *r = &rules[nrules - 1];
	r->verbatim = str_append(r->verbatim, yytext);
}

void handle_verbatimEnd(void)
{
	print_dbg("verbatimEnd: %s", yytext);
}

void handle_unknownOpt(void)
{
	print_warnl("unknown option: %s", yylineno - 1, yytext);
}

void handle_invalid(void)
{
	print_errl("invalid token\n", yylineno - 1);
}
