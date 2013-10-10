#include <bench.h>


perf_t store (stream_t *s) {
    perf_t ret = {s->size, s->size/16};
    if (s->size >= 128) {
	__asm__ __volatile__(
	    "movlpd (%%rbx), %%xmm0;"
	    "mfence;"
	    "_loop:"
#ifdef USE_AVX
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
	    "mfence;"
	    :
	    : "b"(s->stream), "c" (s->size)
#ifdef USE_AVX
	    : "%ymm0"
#else
	    : "%xmm0"
#endif
	    );
    }
    return ret;
}



