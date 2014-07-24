#include <bench.h>


perf_t mbench_ll (stream_t *src1, stream_t *src2) {
    perf_t ret = {0, 0};
    uint64_t unused0, unused1, unused2;

    if (src1->size >= 128) {
	ret.instructions = 2*src1->size / 16;
	ret.bytes = 2*src1->size;
	__asm__ __volatile__(
#ifndef	USE_MIC
	    "mfence;"
#endif
	    "_loop:"
#ifdef USE_MIC
	    "vmovaps (%%rax), %%zmm0;"
	    "vmovaps (%%rbx), %%zmm0;"

	    "vmovaps 64(%%rax), %%zmm0;"
	    "vmovaps 64(%%rbx), %%zmm0;"
#elif (defined USE_AVX)
	    "vmovaps (%%rax), %%ymm0;"
	    "vmovaps (%%rbx), %%ymm1;"

	    "vmovaps 32(%%rax), %%ymm0;"
	    "vmovaps 32(%%rbx), %%ymm1;"

	    "vmovaps 64(%%rax), %%ymm0;"
	    "vmovaps 64(%%rbx), %%ymm1;"

	    "vmovaps 96(%%rax), %%ymm0;"
	    "vmovaps 96(%%rbx), %%ymm1;"
#else
	    "movaps (%%rax), %%xmm0;"
	    "movaps (%%rbx), %%xmm0;"

	    "movaps 16(%%rax), %%xmm0;"
	    "movaps 16(%%rbx), %%xmm0;"

	    "movaps 32(%%rax), %%xmm0;"
	    "movaps 32(%%rbx), %%xmm0;"

	    "movaps 48(%%rax), %%xmm0;"
	    "movaps 48(%%rbx), %%xmm0;"

	    "movaps 64(%%rax), %%xmm0;"
	    "movaps 64(%%rbx), %%xmm0;"

	    "movaps 80(%%rax), %%xmm0;"
	    "movaps 80(%%rbx), %%xmm0;"

	    "movaps 96(%%rax), %%xmm0;"
	    "movaps 96(%%rbx), %%xmm0;"

	    "movaps 112(%%rax), %%xmm0;"
	    "movaps 112(%%rbx), %%xmm0;"
#endif
	    "add $128, %%rax;"
	    "add $128, %%rbx;"
	    "sub $128, %%rcx;"
	    "jnz _loop;"
#ifndef	USE_MIC
	    "mfence;"
#endif
	    : "=a" (unused0), "=b" (unused1), "=c" (unused2)
	    : "a"(src1->stream),
	      "b"(src2->stream), "c" (src2->size)
#ifdef USE_MIC
	    : "%zmm0"
#elif (defined USE_AVX)
	    : "%ymm0", "%ymm1"
#else
	    : "%xmm0"
#endif
	    );
    }
    else {
	ret.instructions = 0;
	ret.bytes = 0;
    }
    return ret;
}
