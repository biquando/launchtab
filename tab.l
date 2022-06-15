%{
#include "launchtab.h"
%}

comment        ^{spc}?#.*{eol}
cronRule       ^{spc}?(({calEntry}{spc}){5}|{specialCal}{spc}).+{eol}
id             ^{spc}?\[.+\]{eol}
emptyLine      ^{spc}?{eol}
commandMulti   ^{spc}?.*\\{eol}
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
calEntry \*|([0-9]+)
specialCal @(reboot|yearly|annually|monthly|weekly|daily|midnight|hourly)

%s INISC
%s COMSC
%s OPTSC
%s VERSC

%%

{comment}              {handle_comment();                    }
<INISC>{cronRule}      {handle_cronRule();                   }
<INISC>{envar}         {handle_globEnvar();                  }
<INISC>{id}            {handle_id();             BEGIN COMSC;}
<OPTSC>{id}            {handle_id();             BEGIN COMSC;}
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
