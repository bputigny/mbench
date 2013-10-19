#ifndef BENCH_H
#define BENCH_H

#include <stdint.h>
#include <libmbench.h>

#define REPS	10

perf_t mbench_load(stream_t *stream);
perf_t mbench_store(stream_t *stream);
perf_t mbench_copy(stream_t *dest, stream_t *src);
perf_t mbench_copy_sse(stream_t *dest, stream_t *src);

perf_t mbench_ls(stream_t *src1, stream_t *dest);
perf_t mbench_ll(stream_t *src1, stream_t *src2);
perf_t mbench_lls(stream_t *src1, stream_t *src2, stream_t *dest);
perf_t mbench_llls(stream_t *src1, stream_t *src2, stream_t *src3, stream_t *dest);

perf_t mbench_barrier();
perf_t mbench_clflush(stream_t *s);

perf_t mbench_latency_load(stream_t *stream);

#endif // BENCH_H
