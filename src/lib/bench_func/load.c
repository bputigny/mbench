#include <bench.h>


perf_t mbench_load (stream_t *s) {
    perf_t ret = {s->size, s->size/16};
    uint64_t unused0, unused1;

    if (s->size >= 128) {
	__asm__ __volatile__(
#ifndef USE_MIC
	    "mfence;"
#endif
	    "_loop:"
#ifdef USE_MIC
	    "vmovaps (%%rbx), %%zmm0;"
	    "vmovaps 64(%%rbx), %%zmm0;"
#elif (defined USE_AVX)
	    "vmovaps (%%rbx), %%ymm0;"
	    "vmovaps 32(%%rbx), %%ymm0;"
	    "vmovaps 64(%%rbx), %%ymm0;"
	    "vmovaps 96(%%rbx), %%ymm0;"
#else
	    "movaps (%%rbx), %%xmm0;"
	    "movaps 16(%%rbx), %%xmm0;"
	    "movaps 32(%%rbx), %%xmm0;"
	    "movaps 48(%%rbx), %%xmm0;"
	    "movaps 64(%%rbx), %%xmm0;"
	    "movaps 80(%%rbx), %%xmm0;"
	    "movaps 96(%%rbx), %%xmm0;"
	    "movaps 112(%%rbx), %%xmm0;"
#endif
	    "add $128, %%rbx;"
	    "sub $128, %%rcx;"
	    "jnz _loop;"
#ifndef USE_MIC
	    "mfence;"
#endif
	    : "=b" (unused0), "=c" (unused1)
	    : "b"(s->stream), "c" (s->size)
#ifdef USE_MIC
            : "%zmm0"
#elif (defined USE_AVX)
	    : "%ymm0"
#else
	    : "%xmm0"
#endif
	    );
    }
    return ret;
}

