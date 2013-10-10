%{
#include <symbol.h>
#include <parser.h>

int line = 1;
%}

D			[0-9]
L			[a-zA-Z_]
SPACE			[ \t]
%%

"thread"		{ return THREAD; }
"time"			{ return TIME; }
"kb"|"kB"|"Kb"|"KB"	{ yylval.num = 1024; return UNIT; }
"runtime"		{ yylval.num = -1; return RUNTIME_SIZE; }
{L}({L}|{D})*		{ yylval.str = strdup (yytext); return ID; }
{D}+			{ yylval.num = atoi (yytext); return NUM; }
{SPACE}			{ /* DO NOTHING	*/ }
\n			{ line++; }
.			{ return *yytext; }

%%

int yywrap () {
	return(1);
}

