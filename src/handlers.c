#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "launchtab.h"
#include "style.h"
#include "util.h"

extern char *yytext;
extern int yylineno;

static struct rule *new_rule(struct tab *t)
{
	t->nrules++;
	t->rules = try_realloc(t->rules, t->nrules * sizeof *t->rules);

	struct rule *r = &t->rules[t->nrules - 1];
	memset(r, 0, sizeof *r);
	return r;
}

#define SPEC_CAL(special, normal) \
	} else if (strncmp(text, special, sizeof special - 1) == 0) { \
		char *tmp = try_malloc(sizeof normal); \
		strcpy(tmp, normal); \
		add_calendar(tmp, r); \
		free(tmp); \
		return text + sizeof special - 1
static char *add_calendar(char *text, struct rule *r)
{
	/* Check special calendar rules */
	if (text[0] == '@') {
		if (strncmp(text, "@reboot", 7) == 0) {
			return text + 7; /* @reboot is not implemented */

		SPEC_CAL("@yearly",   "0 0 1 1 *");
		SPEC_CAL("@annually", "@yearly"  );
		SPEC_CAL("@monthly",  "0 0 1 * *");
		SPEC_CAL("@weekly",   "0 0 * * 0");
		SPEC_CAL("@daily",    "0 0 * * *");
		SPEC_CAL("@midnight", "@daily"   );
		SPEC_CAL("@hourly",   "0 * * * *");
		}
	}

	r->ncals++;
	r->cal = try_realloc(r->cal, r->ncals * sizeof *r->cal);
	struct calendar *c = &r->cal[r->ncals - 1];
	memset(c, 0, sizeof *c);

	int i = -1;
	char *entry;
	for (entry = strtok(text, " \t");
	     entry && i < 4;
	     entry = strtok(NULL, " \t")) {
		i++;
		if (strlen(entry) == 1 && entry[0] == '*')
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
		unsigned int *nvars_p)
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

	(*nvars_p)++;
	*labs_p = try_realloc(*labs_p, *nvars_p * sizeof **labs_p);
	*vals_p = try_realloc(*vals_p, *nvars_p * sizeof **vals_p);

	(*labs_p)[*nvars_p - 1] = try_malloc(label_len);
	(*vals_p)[*nvars_p - 1] = try_malloc(value_len);
	strcpy((*labs_p)[*nvars_p - 1], label);
	strcpy((*vals_p)[*nvars_p - 1], value);
}


void handle_cronRule(struct tab *t)
{
	print_dbg("cronRule: %s", yytext);
	yytext = trim(yytext);

	struct rule *r = new_rule(t);
	r->id = try_malloc(sizeof "cron.rule." + t->ncronrules/10);
	sprintf(r->id, "cron.rule.%u", t->ncronrules++);

	yytext = add_calendar(yytext, r);
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

void handle_globEnvar(struct tab *t)
{
	print_dbg("globEnvar: %s", yytext);
	parse_envar(yytext, &t->varlabels_glob, &t->varvalues_glob,
			&t->nvars_glob);
}

void handle_id(struct tab *t)
{
	print_dbg("id: %s", yytext);
	yytext = trim(yytext);

	struct rule *r = new_rule(t);

	int len = strlen(yytext);
	yytext[len - 1] = '\0'; /* remove trailing ] */
	r->id = try_malloc(len);
	strcpy(r->id, yytext + 1); /* remove leading [ */
}

void handle_comment(struct tab *t)
{
	print_dbg("comment: %s", yytext);
}

void handle_emptyLine(struct tab *t)
{
	print_dbg("emptyLine: %s", yytext);
}

void handle_commandStart(struct tab *t)
{
	print_dbg("commandStart: %s", yytext);
}

void handle_commandEnd(struct tab *t)
{
	print_dbg("commandEnd: %s", yytext);
}

void handle_commandCont(struct tab *t)
{
	handle_command(t);
}

void handle_command(struct tab *t)
{
	print_dbg("command: %s", yytext);
	yytext = trim(yytext);
	int len = strlen(yytext);
	if (len > 1 && yytext[len-1] == '\\') {
		yytext[len-1] = '\0'; /* remove trailing backslash */
	}

	struct rule *r = &t->rules[t->nrules - 1];
	r->command = str_append(r->command, yytext);
}

void handle_commandMulti(struct tab *t)
{
	print_dbg("commandMulti: %s", yytext);
	struct rule *r = &t->rules[t->nrules - 1];
	r->command = str_append(r->command, yytext);
	// r->command = str_append(r->command, "\n");
}

/* Options */

void handle_interval(struct tab *t)
{
	print_dbg("interval: %s", yytext);
	yytext = trim(yytext);
	yytext += sizeof("Interval");
	yytext = trim(yytext);

	struct rule *r = &t->rules[t->nrules - 1];
	r->interval = try_realloc(r->interval, strlen(yytext) + 1);
	strcpy(r->interval, yytext);
}

void handle_calendar(struct tab *t)
{
	print_dbg("calendar: %s", yytext);
	yytext = trim(yytext);

	add_calendar(yytext, &t->rules[t->nrules - 1]);
}

void handle_envar(struct tab *t)
{
	print_dbg("envar: %s", yytext);
	struct rule *r = &t->rules[t->nrules - 1];
	parse_envar(yytext, &r->varlabels, &r->varvalues, &r->nvars);
}

void handle_stdin(struct tab *t)
{
	print_dbg("stdin: %s", yytext);
	yytext = trim(yytext);
	yytext += sizeof "<" - 1;
	yytext = trim(yytext);

	struct rule *r = &t->rules[t->nrules - 1];
	r->fd[0] = try_realloc(r->fd[0], strlen(yytext) + 1);
	strcpy(r->fd[0], yytext);
}

void handle_stdout(struct tab *t)
{
	print_dbg("stdout: %s", yytext);
	yytext = trim(yytext);
	yytext += sizeof ">" - 1;
	yytext = trim(yytext);

	struct rule *r = &t->rules[t->nrules - 1];
	r->fd[1] = try_realloc(r->fd[1], strlen(yytext) + 1);
	strcpy(r->fd[1], yytext);
}

void handle_stderr(struct tab *t)
{
	print_dbg("stderr: %s", yytext);
	yytext = trim(yytext);
	yytext += sizeof "2>" - 1;
	yytext = trim(yytext);

	struct rule *r = &t->rules[t->nrules - 1];
	r->fd[2] = try_realloc(r->fd[2], strlen(yytext) + 1);
	strcpy(r->fd[2], yytext);
}


void handle_verbatimStart(struct tab *t)
{
	print_dbg("verbatimStart: %s", yytext);
}

void handle_verbatimLine(struct tab *t)
{
	print_dbg("verbatimLine: %s", yytext);
	struct rule *r = &t->rules[t->nrules - 1];
	r->verbatim = str_append(r->verbatim, yytext);
}

void handle_verbatimEnd(struct tab *t)
{
	print_dbg("verbatimEnd: %s", yytext);
}

void handle_unknownOpt(struct tab *t)
{
	print_warnl("unknown option: %s", yylineno - 1, yytext);
}

void handle_invalid(struct tab *t)
{
	print_errl("invalid token\n", yylineno);
	t->invalid = 1;
}
