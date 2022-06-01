%{
#include <stdio.h>
#include "launchtab.h"
%}

id             ^{space}?\[.+\]$
comment        ^{space}?#.*$
commandMulti   ^{space}?.*\\$
command        ^{space}?.*$
interval       ^{space}?Interval{space}[0-9]+$
calendar       ^{space}?({calEntry}{space}){0,4}{calEntry}$
envar          ^{space}?[a-zA-Z0-9_]+=.*$
stdin          ^{space}?\<.+$
stdout         ^{space}?\>.+$
stderr         ^{space}?2\>.+$
verbatimStart  ^{space}?---$
verbatimEnd    ^{space}?---$
verbatimLine   ^{space}?.*$
unknownOpt     ^{space}?.*$

space [ \t]+
calEntry (\*|([0-9]+))

%s COMSC
%s OPTSC
%x VERSC

%%

<OPTSC>{id} {printf("id: %s", yytext); BEGIN COMSC;}
{comment} {printf("comment: %s", yytext);}
<COMSC>{commandMulti} {printf("commandMulti: %s", yytext);}
<COMSC>{command} {printf("command: %s", yytext); BEGIN OPTSC;}

<OPTSC>{interval} {printf("interval: %s", yytext);}
<OPTSC>{calendar} {printf("calendar: %s", yytext);}
<OPTSC>{envar} {printf("envar: %s", yytext);}
<OPTSC>{stdin} {printf("stdin: %s", yytext);}
<OPTSC>{stdout} {printf("stdout: %s", yytext);}
<OPTSC>{stderr} {printf("stderr: %s", yytext);}

<OPTSC>{verbatimStart} {printf("verbatimStart: %s", yytext); BEGIN VERSC;}
<VERSC>{verbatimEnd} {printf("verbatimEnd: %s", yytext); BEGIN OPTSC;}
<VERSC>{verbatimLine} {printf("verbatimLine: %s", yytext);}
<OPTSC>{unknownOpt} {printf("unknownOpt: %s", yytext);}

%%

int yywrap(void)
{
	return 1;
}

void lex_init(void)
{
	BEGIN OPTSC;
}
