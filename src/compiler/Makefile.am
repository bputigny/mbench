BUILT_SOURCES	= parser.h

AM_YFLAGS	= -d
AM_CFLAGS	= -I$(srcdir)/include


bin_SCRIPTS	= mbench_bc
CLEANFILES	= $(bin_SCRIPTS)
bin_PROGRAMS	= mbench_cgen
mbench_cgen_SOURCES	= parser.y scanner.l code_generator.c symbol.c include/symbol.h
