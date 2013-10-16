#include <bench.h>


perf_t barrier() {
    perf_t ret = {0, 0};
#pragma omp barrier
    return ret;
}
