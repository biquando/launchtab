#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "launchtab.h"

extern char *yytext;
extern int yylineno;

/* Remove leading and trailing whitespace */
static char *trim(char *str)
{
	if (!str)
		return str;

	while (isspace(*str)) str++;

	if (*str) {
		char *end = str + strlen(str) - 1;
		while (end > str && isspace(*end)) end--;
		end[1] = '\0';
	}

	return str;
}

void *try_malloc(size_t size)
{
	void *ptr = malloc(size);
	if (!ptr)
		exit(6);
	return ptr;
}

void *try_realloc(void *ptr, size_t size)
{
	ptr = realloc(ptr, size);
	if (!ptr)
		exit(6);
	return ptr;
}


void handle_id(void)
{
	printf("id: %s", yytext);
	yytext = trim(yytext);

	nrules++;
	rules = try_realloc(rules, nrules * sizeof *rules);

	struct rule *r = &rules[nrules - 1];
	memset(r, 0, sizeof *r);

	int len = strlen(yytext);
	yytext[len - 1] = '\0'; /* remove trailing ] */
	r->id = try_malloc(len);
	strcpy(r->id, yytext + 1); /* remove leading [ */
}

void handle_comment(void)
{
	printf("comment: %s", yytext);
}

void handle_emptyLine(void)
{
	printf("emptyLine: %s", yytext);
}

void handle_commandMulti(void)
{
	handle_command();
}

void handle_command(void)
{
	printf("command: %s", yytext);
	yytext = trim(yytext);
	int len = strlen(yytext);
	if (len > 1 && yytext[len-1] == '\\') {
		yytext[len-1] = '\0'; /* remove trailing backslash */
	}

	struct rule *r = &rules[nrules - 1];
	if (r->command) {
		int l1 = strlen(r->command);
		int l2 = strlen(yytext);
		r->command = try_realloc(r->command, l1 + l2 + 1);
		strcpy(r->command + l1, yytext);
	} else {
		r->command = try_realloc(r->command, strlen(yytext) + 1);
		strcpy(r->command, yytext);
	}
}


/* Options */

void handle_interval(void)
{
	printf("interval: %s", yytext);
	yytext = trim(yytext);
	yytext += sizeof("Interval");
	yytext = trim(yytext);

	struct rule *r = &rules[nrules - 1];
	r->interval = try_realloc(r->interval, strlen(yytext) + 1);
	strcpy(r->interval, yytext);
}

void handle_calendar(void)
{
	printf("calendar: %s", yytext);
	yytext = trim(yytext);

	struct rule *r = &rules[nrules - 1];
	r->ncal++;
	r->cal = try_realloc(r->cal, r->ncal * sizeof *r->cal);
	struct calendar *c = &r->cal[r->ncal - 1];
	memset(c, 0, sizeof *c);

	int i = -1;
	for (char *entry = strtok(yytext, " \t");
	     entry;
	     entry = strtok(NULL, " \t")) {
		i++;
		if (entry[0] == '*')
			continue;
		c->ent[i] = try_malloc(strlen(entry) + 1);
		strcpy(c->ent[i], entry);
	}
}

void handle_envar(void)
{
	printf("envar: %s", yytext);

	char *label = yytext;
	while (isspace(*label)) label++;

	char *value = label;
	while (*value != '=') value++;
	*value = '\0';
	value++;

	int labellen = strlen(label);
	int valuelen = strlen(value);
	value[valuelen - 1] = '\0';

	struct rule *r = &rules[nrules - 1];
	r->nvar++;
	r->varlabels = try_realloc(r->varlabels,r->nvar * sizeof *r->varlabels);
	r->varvalues = try_realloc(r->varvalues,r->nvar * sizeof *r->varvalues);

	r->varlabels[r->nvar-1] = try_malloc(labellen);
	r->varvalues[r->nvar-1] = try_malloc(valuelen);
	strcpy(r->varlabels[r->nvar-1], label);
	strcpy(r->varvalues[r->nvar-1], value);
}

void handle_stdin(void)
{
	printf("stdin: %s", yytext);
	yytext = trim(yytext);
	yytext += sizeof "<" - 1;
	yytext = trim(yytext);

	struct rule *r = &rules[nrules - 1];
	r->fd[0] = try_realloc(r->fd[0], strlen(yytext) + 1);
	strcpy(r->fd[0], yytext);
}

void handle_stdout(void)
{
	printf("stdout: %s", yytext);
	yytext = trim(yytext);
	yytext += sizeof ">" - 1;
	yytext = trim(yytext);

	struct rule *r = &rules[nrules - 1];
	r->fd[1] = try_realloc(r->fd[1], strlen(yytext) + 1);
	strcpy(r->fd[1], yytext);
}

void handle_stderr(void)
{
	printf("stderr: %s", yytext);
	yytext = trim(yytext);
	yytext += sizeof "2>" - 1;
	yytext = trim(yytext);

	struct rule *r = &rules[nrules - 1];
	r->fd[2] = try_realloc(r->fd[2], strlen(yytext) + 1);
	strcpy(r->fd[2], yytext);
}


void handle_verbatim(void)
{
	printf("verbatim: %s", yytext);
	yytext += sizeof "---";

	struct rule *r = &rules[nrules - 1];
	int len = strlen(yytext) - sizeof "---";
	yytext[len] = '\0';
	r->verbatim = try_realloc(r->verbatim, len + 1);
	strcpy(r->verbatim, yytext);
}

void handle_unknownOpt(void)
{
	printf("unknownOpt: %s", yytext);
}

void handle_invalid(void)
{
	printf("(%d) invalid: %s\n", yylineno, yytext);
}
