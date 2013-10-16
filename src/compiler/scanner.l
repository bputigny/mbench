%{
#include <symbol.h>
#include <parser.h>

int line = 1;
int comment = 0;
%}

D			[0-9]
L			[a-zA-Z_]
SPACE		[ \t]
%%

"#"				{ comment = 1; }
"thread"		{ if (!comment) return THREAD; }
"time"			{ if (!comment) return TIME; }
"kb"|"kB"|"Kb"|"KB"	{ if (!comment) { yylval.num = 1024; return UNIT;} }
"mb"|"mB"|"Mb"|"MB"	{ if (!comment) {yylval.num = 1024*1024; return UNIT;} }
"gb"|"gB"|"Gb"|"GB"	{ if (!comment) {yylval.num = 1024*1024*1024; return UNIT;} }
"runtime"		{ if (!comment) {yylval.num = -1; return RUNTIME_SIZE;} }
{L}({L}|{D})*	{ if (!comment) {yylval.str = strdup (yytext); return ID;} }
{D}+			{ if (!comment) {yylval.num = atoi (yytext); return NUM;} }
{SPACE}			{ /* DO NOTHING	*/ }
\n			{ line++; comment = 0; }
.			{ if (!comment) return *yytext; }

%%

int yywrap () {
	return(1);
}

