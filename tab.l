%{
#include <stdio.h>
%}

id             ^{space}?\[.+\]$
comment        ^{space}?#.*$
command        ^{space}?.*$
interval       ^{space}?Interval{space}[0-9]+$
calendar       ^{space}?((\*|([0-9]+)){space}){0,4}(\*|([0-9]+))$
envar          ^{space}?[a-zA-Z0-9_]+=.*$
stdin          ^{space}?\<.+$
stdout         ^{space}?\>.+$
stderr         ^{space}?2\>.+$
verbatimStart  ^{space}?---$
verbatimEnd    ^{space}?---$
verbatimLine   ^{space}?.*$
unknownOpt     ^{space}?.*$
space [ \t]+

%s COMSC
%s OPTSC
%x VERSC

%%

<OPTSC>{id} {printf("id: %s", yytext); BEGIN COMSC;}
{comment} {printf("comment: %s", yytext);}
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
