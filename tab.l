%{
#include "launchtab.h"
%}

cronRule       ^{space}?({calEntry}{space}){5}.+\n
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
verbatim       ^---\n(.|\n)*\n---\n
unknownOpt     ^{space}?.*\n

space [ \t]+
calEntry (\*|([0-9]+))

%s INISC
%s COMSC
%s OPTSC

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

<OPTSC>{verbatim}      {handle_verbatim();                   }
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
