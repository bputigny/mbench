/* -*-mode: c-mode-*- */

%{
#define _GNU_SOURCE
#include <stdio.h>

#include <symbol.h>
#include <parser.h>

extern int yylex ();
extern int yyerror (char *s);

stream_tab_t *stream_tab = NULL;
run_tab_t *run_tab = NULL;
unsigned thread_id_max = 0;
%}

%union {
    int num;
    char *str;
    stream_t stream;
    func_t func;
    run_t run;
    run_tab_t *run_l;
    binding_t binding;
    num_tab_t *num_table;
}

%token <num> NUM UNIT
%token <str> ID
%token THREAD TIME RUNTIME_SIZE ALLOCATED_BY

%type <num>         size numa_allocator
%type <stream>      def
%type <run>         run basic_run
%type <run_l>       run_lst
%type <func>        function
%type <binding>     binding
%type <num_table>   num_lst
%type <str>         arg_lst

%start benchmark

%%

benchmark
: stream_def_lst run_lst        {run_tab = $2;}
;

stream_def_lst
:  stream_def_lst def ';'       {stream_table_add (stream_tab, $2);}
|
;

def
: ID '=' size numa_allocator    {   $$.id = $1;
                                    $$.alloc_by = $4;
                                    if ($3==-1) {
                                        $$.type=DYNAMIC;
                                    }
                                    else {
                                        $$.byte_size = $3; $$.type=STATIC;
                                    }
                                }
;

numa_allocator
: ALLOCATED_BY NUM              {  $$ = $2; if ($2>thread_id_max) thread_id_max = $2; }
|                               { $$ = 0; } // if not specified alloc with OMP master
;

size
: NUM UNIT				{$$ = $1 * $2;}
| RUNTIME_SIZE				{$$ = -1;}
;

run_lst
: run_lst run ';'			{run_table_add ($1, $2); $$ = $1;}
| run ';'				{$$ = run_table_new (); run_table_add ($$, $1);}
;

run
: basic_run				{$$ = $1;}
| TIME '('run_lst ')'			{$$.l = $3; $$.timer = 1;}
;

basic_run
: THREAD ':' binding '.' function	{$$.binding = $3; $$.func = $5;}
;

binding
: num_lst	       			{$$.num_table = $1;; $$.type=LIST;}
| NUM '-' NUM				{$$.start = $1; $$.stop = $3; $$.type=RANGE; if ($3>thread_id_max) thread_id_max = $3;}
;

num_lst
: num_lst ',' NUM			{num_table_add ($$, $3); if ($3>thread_id_max) thread_id_max = $3; }
| NUM					{$$ = num_table_new (); num_table_add ($$, $1); if ($1>thread_id_max) thread_id_max = $1;}
;

function
: ID '(' arg_lst ')'			{$$.name = $1; $$.arg = $3;}
| ID '(' ')'					{$$.name = $1; $$.arg = "";}
;

arg_lst
: ID					{$$ = $1;}
| arg_lst ',' ID			{asprintf (&$$, "%s,%s", $1, $3);}
;

%%

#include <getopt.h>

extern int line;
extern FILE *yyin;

char *file_name = "STDIN";
FILE *outfile = NULL;

int yyerror (char *s) {
    fprintf (stderr, "\n%s at in %s line %d\n\n", s, file_name, line);
    exit (1);
}

extern void code_generator();

int main (int argc, char* argv[]) {
    outfile = stdout;

    char c;
    while ((c = getopt (argc, argv, "o:")) != EOF) {
	switch (c) {
	case 'o':
	    outfile = fopen (optarg, "w");
	    break;
	}
    }
    if (optind < argc) {
	int file = 0;
        while ((optind + file) < argc) {
	    yyin = fopen (argv[optind + file], "r");
	    if (!yyin) {
		fprintf (stderr, "Error: cannot open input file %s\n", argv[optind + file]);
		exit (EXIT_FAILURE);
	    }
	    file_name = argv[optind + file];
	    file ++;
	}
	if (file != 1) {
	    fprintf (stderr, "Error: too many benchmark files\n");
	    exit (EXIT_FAILURE);
	}
    }

    stream_tab = stream_table_new ();

    yyparse ();

    code_generator ();

    free (stream_tab);

    fclose (outfile);
    fclose (yyin);
    return 0;
}
