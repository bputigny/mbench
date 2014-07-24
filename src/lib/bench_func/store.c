#include <bench.h>


perf_t mbench_store (stream_t *s) {
    perf_t ret = {s->size, s->size/16};
    uint64_t unused0, unused1;

    if (s->size >= 128) {
	__asm__ __volatile__(
/*	    "movlpd (%%rbx), %%xmm0;" dead code, was used for pointer chasing in old revisions */
#ifndef USE_MIC
	    "mfence;"
#endif
	    "_loop:"
#ifdef USE_MIC
	    "vmovaps %%zmm0, (%%rbx);"
	    "vmovaps %%zmm0, 64(%%rbx);"
#elif (defined USE_AVX)
	    "vmovaps %%ymm0, (%%rbx);"
	    "vmovaps %%ymm0, 32(%%rbx);"
	    "vmovaps %%ymm0, 64(%%rbx);"
	    "vmovaps %%ymm0, 96(%%rbx);"
#else
	    "movaps %%xmm0, (%%rbx);"
	    "movaps %%xmm0, 16(%%rbx);"
	    "movaps %%xmm0, 32(%%rbx);"
	    "movaps %%xmm0, 48(%%rbx);"
	    "movaps %%xmm0, 64(%%rbx);"
	    "movaps %%xmm0, 80(%%rbx);"
	    "movaps %%xmm0, 96(%%rbx);"
	    "movaps %%xmm0, 112(%%rbx);"
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



