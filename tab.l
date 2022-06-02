%{
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "launchtab.h"
%}

id             ^\[.+\]\n
comment        ^{space}?#.*\n
emptyLine      ^{space}?\n
commandMulti   ^{space}?.*\\\n
command        ^{space}?.*\n
interval       ^{space}?Interval{space}[0-9]+\n
calendar       ^{space}?({calEntry}{space}){0,4}{calEntry}\n
envar          ^{space}?[a-zA-Z0-9_]+=.*\n
stdin          ^{space}?\<.+\n
stdout         ^{space}?\>.+\n
stderr         ^{space}?2\>.+\n
verbatimStart  ^{space}?---\n
verbatimEnd    ^{space}?---\n
verbatimLine   ^{space}?.*\n
unknownOpt     ^{space}?.*\n

space [ \t]+
calEntry (\*|([0-9]+))

%s COMSC
%s OPTSC
%x VERSC

%%

<OPTSC>{id}            {handle_id();             BEGIN COMSC;}
{comment}              {handle_comment();                    }
{emptyLine}            {handle_emptyLine();                  }
<COMSC>{commandMulti}  {handle_commandMulti();               }
<COMSC>{command}       {handle_command();        BEGIN OPTSC;}

<OPTSC>{interval}      {handle_interval();                   }
<OPTSC>{calendar}      {handle_calendar();                   }
<OPTSC>{envar}         {handle_envar();                      }
<OPTSC>{stdin}         {handle_stdin();                      }
<OPTSC>{stdout}        {handle_stdout();                     }
<OPTSC>{stderr}        {handle_stderr();                     }

<OPTSC>{verbatimStart} {handle_verbatimStart();  BEGIN VERSC;}
<VERSC>{verbatimEnd}   {handle_verbatimEnd();    BEGIN OPTSC;}
<VERSC>{verbatimLine}  {handle_verbatimLine();               }
<OPTSC>{unknownOpt}    {handle_unknownOpt();                 }

<*>.|\n {}

%%

int yywrap(void)
{
	return 1;
}

void lex_init(void)
{
	BEGIN OPTSC;
}


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

static void *try_malloc(size_t size)
{
	void *ptr = malloc(size);
	if (!ptr)
		exit(6);
	return ptr;
}

static void *try_realloc(void *ptr, size_t size)
{
	ptr = realloc(ptr, size);
	if (!ptr)
		exit(6);
	return ptr;
}


void handle_id(void)
{
	printf("id: %s", yytext);
	nrules++;
	rules = try_realloc(rules, nrules * sizeof *rules);

	struct rule *r = &rules[nrules - 1];
	memset(r, 0, sizeof *r);

	yytext = trim(yytext);
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
		yytext[len-2] = '\0'; /* remove trailing backslash */
	}

	struct rule *r = &rules[nrules - 1];
	if (r->command) {
		int l1 = strlen(r->command);
		int l2 = strlen(yytext);
		r->command = try_realloc(r->command, l1 + l2 + 1);
		strcpy(r->command + l1, yytext);
	} else {
		r->command = try_malloc(strlen(yytext) + 1);
		strcpy(r->command, yytext);
	}
}


void handle_interval(void)
{
	printf("interval: %s", yytext);
}

void handle_calendar(void)
{
	printf("calendar: %s", yytext);
}

void handle_envar(void)
{
	printf("envar: %s", yytext);
}

void handle_stdin(void)
{
	printf("stdin: %s", yytext);
}

void handle_stdout(void)
{
	printf("stdout: %s", yytext);
}

void handle_stderr(void)
{
	printf("stderr: %s", yytext);
}


void handle_verbatimStart(void)
{
	printf("verbatimStart: %s", yytext);
}

void handle_verbatimEnd(void)
{
	printf("verbatimEnd: %s", yytext);
}

void handle_verbatimLine(void)
{
	printf("verbatimLine: %s", yytext);
}

void handle_unknownOpt(void)
{
	printf("unknownOpt: %s", yytext);
}
