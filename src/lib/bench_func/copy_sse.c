#include <bench.h>


perf_t mbench_copy_sse (stream_t *dest, stream_t *src) {
    perf_t ret = {src->size, 2*src->size / 16};
    uint64_t unused0, unused1, unused2;

    if (dest->size >= 128) {
	__asm__ __volatile__(
#ifdef USE_MIC
	    "_loop:"
	    "vmovaps (%%rbx), %%zmm0;"
	    "vmovaps %%zmm0, (%%rax);"

	    "vmovaps 64(%%rbx), %%zmm0;"
	    "vmovaps %%zmm0, 64(%%rax);"

	    "add $128, %%rax;"
	    "add $128, %%rbx;"
	    "sub $128, %%rcx;"
	    "jnz _loop;"
#else
	    "mfence;"
	    "_loop:"
	    "movaps (%%rbx), %%xmm0;"
	    "movaps %%xmm0, (%%rax);"

	    "movaps 16(%%rbx), %%xmm0;"
	    "movaps %%xmm0, 16(%%rax);"

	    "movaps 32(%%rbx), %%xmm0;"
	    "movaps %%xmm0, 32(%%rax);"

	    "movaps 48(%%rbx), %%xmm0;"
	    "movaps %%xmm0, 48(%%rax);"

	    "movaps 64(%%rbx), %%xmm0;"
	    "movaps %%xmm0, 64(%%rax);"

	    "movaps 80(%%rbx), %%xmm0;"
	    "movaps %%xmm0, 80(%%rax);"

	    "movaps 96(%%rbx), %%xmm0;"
	    "movaps %%xmm0, 96(%%rax);"

	    "movaps 112(%%rbx), %%xmm0;"
	    "movaps %%xmm0, 112(%%rax);"

	    "add $128, %%rax;"
	    "add $128, %%rbx;"
	    "sub $128, %%rcx;"
	    "jnz _loop;"
	    "mfence;"
#endif
	    : "=a" (unused0), "=b" (unused1), "=c" (unused2)
	    : "a" (dest->stream), "b"(src->stream), "c" (dest->size)
#ifdef USE_MIC
	    : "%zmm0"
#else
	    : "%xmm0"
#endif
	    );
    }
    return ret;
}
