#ifndef BENCH_H
#define BENCH_H

#include <stdint.h>
#include <libmbench.h>

#define REPS	10

perf_t load (stream_t *stream);
perf_t store (stream_t *stream);
perf_t copy (stream_t *dest, stream_t *src);
perf_t copy_sse (stream_t *dest, stream_t *src);

perf_t ls (stream_t *src1, stream_t *dest);
perf_t ll (stream_t *src1, stream_t *src2);
perf_t lls (stream_t *src1, stream_t *src2, stream_t *dest);
perf_t llls (stream_t *src1, stream_t *src2, stream_t *src3, stream_t *dest);

perf_t mbench_ddot (stream_t *s0, stream_t *s1);
perf_t mbench_daxpy (stream_t *s0, stream_t *s1);

perf_t latency_load (stream_t *stream);

#endif // BENCH_H
