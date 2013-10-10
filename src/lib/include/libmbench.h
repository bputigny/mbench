#ifndef LIB_MBENCH_H
#define LIB_MBENCH_H

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h> 

#define TIMER_START	{t = rdtsc (); p.bytes = 0; p.instructions = 0;}
#define TIMER_STOP	{t = rdtsc () - t;}

#define BEGIN_SIZE_LOOP							\
    mbench_progress_bar (0);						\
    uint64_t cs;							\
    for (cs=mbench_size;						\
	 cs<=mbench_size_max; cs*=2) {					\
    for (mbench_size=cs; mbench_size<cs*2 && mbench_size<=mbench_size_max; mbench_size += cs/8) { \
    
#define END_SIZE_LOOP						\
    mbench_progress_bar (mbench_size/(float)mbench_size_max);	\
    } } printf ("\n");


extern uint64_t mbench_size;
extern uint64_t mbench_size_range;
extern uint64_t mbench_size_max;
extern size_t *mbench_stream_alignment;
extern int mbench_num_stream_alignment;

typedef struct {
    uint64_t bytes;
    uint64_t instructions;
} perf_t;

typedef struct {
    char *buffer;
    char *stream;
    uint64_t size;
    uint64_t allocated;
} stream_t;

typedef struct {
    perf_t perf;
    uint64_t runtime;
} run_t;

stream_t *mbench_stream_new (uint64_t size, size_t align);
void mbench_stream_free (stream_t *s);
inline uint64_t rdtsc ();
uint64_t mbench_get_cpu_freq ();
void mbench_parse_args (int argc, char *argv[]);
void mbench_flush_stream (stream_t *s);

void mbench_add_run (run_t *tab, perf_t perf, uint64_t c, int rep, int th);

void mbench_print_results (run_t *run_table, int nthreads);
void mbench_cleanup (int nthreads);
void mbench_progress_bar (float percent);

#ifdef USE_PAPI
void mbench_init_hwcounters (int nthreads);
void mbench_start_hwcounters ();
void mbench_stop_hwcounters (int thread_id, int r);
#endif

#endif // LIB_MBENCH_H
