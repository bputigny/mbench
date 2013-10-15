AM_CFLAGS	= -I$(srcdir)/include

lib_LTLIBRARIES	= libmbench.la
libmbench_la_SOURCES	= libmbench.c bench_func/load.c bench_func/store.c \
			bench_func/latency_load.c bench_func/copy.c bench_func/copy_sse.c \
			bench_func/ls.c bench_func/ll.c bench_func/lls.c bench_func/llls.c \
			bench_func/barrier.c

library_includedir	= $(includedir)/
library_include_HEADERS	= $(srcdir)/include/libmbench.h $(srcdir)/include/bench.h
