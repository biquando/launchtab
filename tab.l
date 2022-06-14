%{
#include "launchtab.h"
%}

cronRule       ^{space}?({calEntry}{space}){5}.+\n
id             ^\[[^\n]+\]\n
comment        ^{space}?#[^\n]*\n
emptyLine      ^{space}?\n
commandMulti   ^{space}?[^\n]*\\\n
command        ^{space}?[^\n]*\n
interval       ^{space}?Interval{space}[0-9]+\n
calendar       ^{space}?({calEntry}{space}){0,4}{calEntry}\n
envar          ^{space}?[a-zA-Z0-9_]+=[^\n]*\n
stdin          ^{space}?\<[^\n]+\n
stdout         ^{space}?\>[^\n]+\n
stderr         ^{space}?2\>[^\n]+\n
verbatimStart  ^---\n
verbatimEnd    ^---\n
verbatimLine   ^[^\n]*\n
unknownOpt     ^{space}?[^\n]*\n

space [ \t]+
calEntry (\*|([0-9]+))

%s INISC
%s COMSC
%s OPTSC
%s VERSC

%%

<INISC>{cronRule}      {handle_cronRule();                   }
<INISC>{id}            {handle_id();             BEGIN COMSC;}
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

<*>.|\n                {handle_invalid();      yyterminate();}

%%

int yywrap(void)
{
	return 1;
}

void lex_init(void)
{
	BEGIN INISC;
}
