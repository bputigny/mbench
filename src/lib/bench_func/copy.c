#include <string.h>
#include <bench.h>


perf_t mbench_copy (stream_t *dest, stream_t *src) {
    perf_t ret = {src->size, 2*src->size / 16};

    memcpy (dest->stream, src->stream, src->size);

    return ret;
}
