%{
#include "launchtab.h"

static struct tab *curr_tab;
%}

comment        ^{spc}?#.*{eol}
cronRuleStart  ^{spc}?{cronCalendar}\{{eol}
cronRuleEnd    ^{spc}?\}{eol}
cronRuleCont   ^{spc}?{cronCalendar}.+\\{eol}
cronRule       ^{spc}?{cronCalendar}.+{eol}
id             ^{spc}?\[[.@_\-0-9A-Za-z]+\]{eol}
emptyLine      ^{spc}?{eol}
commandStart   ^{spc}?\{{eol}
commandEnd     ^{spc}?\}{eol}
commandCont    ^{spc}?.*\\{eol}
command        ^{spc}?.*{eol}
interval       ^{spc}?Interval{spc}[0-9]+{eol}
calendar       ^{spc}?(({calEntry}{spc}){0,4}{calEntry}|{specialCal}){eol}
envar          ^{spc}?[^\n \t=]+{spc}?={spc}?.*{eol}
stdin          ^{spc}?\<.+{eol}
stdout         ^{spc}?\>.+{eol}
stderr         ^{spc}?2\>.+{eol}
verbatimStart  ^{spc}?---{eol}
verbatimEnd    ^{spc}?---{eol}
verbatimLine   ^{spc}?.*{eol}
unknownOpt     ^{spc}?{spc}?.*{eol}

spc [ \t]+
eol {spc}?\n
calEntry [*0-9\-,/]+
cronCalendar ({calEntry}{spc}){5}|{specialCal}{spc}
specialCal @(reboot|yearly|annually|monthly|weekly|daily|midnight|hourly)

/* Initial */
%s INISC

/* Cron command, Cron multicommand */
%s CCOSC
%s CMUSC

/* Command, Multicommand */
%s COMSC
%s MULSC

/* Option */
%s OPTSC

/* Verbatim */
%s VERSC

%%

{comment}              {handle_comment(curr_tab);                    }

<INISC>{cronRuleStart} {handle_cronRuleStart(curr_tab);  BEGIN CMUSC;}
<CMUSC>{cronRuleEnd}   {handle_cronRuleEnd(curr_tab);    BEGIN INISC;}
<INISC>{cronRuleCont}  {handle_cronRuleCont(curr_tab);   BEGIN CCOSC;}
<INISC>{cronRule}      {handle_cronRule(curr_tab);                   }
<CCOSC>{commandCont}   {handle_commandCont(curr_tab);                }
<CCOSC>{command}       {handle_command(curr_tab);        BEGIN INISC;}
<CMUSC>{command}       {handle_commandMulti(curr_tab);               }

<INISC>{envar}         {handle_globEnvar(curr_tab);                  }
<INISC>{id}            {handle_id(curr_tab);             BEGIN COMSC;}
<OPTSC>{id}            {handle_id(curr_tab);             BEGIN COMSC;}
{emptyLine}            {handle_emptyLine(curr_tab);                  }

<COMSC>{commandStart}  {handle_commandStart(curr_tab);   BEGIN MULSC;}
<MULSC>{commandEnd}    {handle_commandEnd(curr_tab);     BEGIN OPTSC;}
<COMSC>{commandCont}   {handle_commandCont(curr_tab);                }
<COMSC>{command}       {handle_command(curr_tab);        BEGIN OPTSC;}
<MULSC>{command}       {handle_commandMulti(curr_tab);               }

<OPTSC>{verbatimStart} {handle_verbatimStart(curr_tab);  BEGIN VERSC;}
<OPTSC>{interval}      {handle_interval(curr_tab);                   }
<OPTSC>{calendar}      {handle_calendar(curr_tab);                   }
<OPTSC>{envar}         {handle_envar(curr_tab);                      }
<OPTSC>{stdin}         {handle_stdin(curr_tab);                      }
<OPTSC>{stdout}        {handle_stdout(curr_tab);                     }
<OPTSC>{stderr}        {handle_stderr(curr_tab);                     }

<VERSC>{verbatimEnd}   {handle_verbatimEnd(curr_tab);    BEGIN OPTSC;}
<VERSC>{verbatimLine}  {handle_verbatimLine(curr_tab);               }
<OPTSC>{unknownOpt}    {handle_unknownOpt(curr_tab);                 }

<*>.|\n                {handle_invalid(curr_tab);      yyterminate();}

%%

int yywrap(void)
{
	return 1;
}

void lex_tab(FILE *f, struct tab *t)
{
	yyin = f;
	yylineno = 1;
	curr_tab = t;
	BEGIN INISC;
	yylex();
}
