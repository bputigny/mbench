#include <bench.h>


perf_t load (stream_t *s) {
    perf_t ret = {s->size, s->size/16};
    if (s->size >= 128) {
	__asm__ __volatile__(
	    "mfence;"
	    "_loop:"
#ifdef USE_AVX
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
