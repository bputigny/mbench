#include <bench.h>


perf_t mbench_llls (stream_t *dest, stream_t *src1, stream_t *src2, stream_t *src3) {
    perf_t ret = {0, 0};
    uint64_t unused0, unused1, unused2, unused3, unused4;

    if (dest->size >= 128) {
	ret.instructions = 2*dest->size / 16;
	ret.bytes = 4*dest->size;
	__asm__ __volatile__(
#ifndef	USE_MIC
	    "mfence;"
#endif
	    "_loop:"
#ifdef USE_MIC
	    "vmovaps (%1), %%zmm0;"
	    "vmovaps (%2), %%zmm0;"
	    "vmovaps (%3), %%zmm0;"
	    "vmovaps %%zmm0, (%0);"

	    "vmovaps 64(%1), %%zmm0;"
	    "vmovaps 64(%2), %%zmm0;"
	    "vmovaps 64(%3), %%zmm0;"
	    "vmovaps %%zmm0, 64(%0);"
#elif (defined USE_AVX)
	    "vmovaps (%1), %%ymm0;"
	    "vmovaps (%2), %%ymm0;"
	    "vmovaps (%3), %%ymm0;"
	    "vmovaps %%ymm0, (%0);"

	    "vmovaps 32(%1), %%ymm0;"
	    "vmovaps 32(%2), %%ymm0;"
	    "vmovaps 32(%3), %%ymm0;"
	    "vmovaps %%ymm0, 32(%0);"

	    "vmovaps 64(%1), %%ymm0;"
	    "vmovaps 64(%2), %%ymm0;"
	    "vmovaps 64(%3), %%ymm0;"
	    "vmovaps %%ymm0, 64(%0);"

	    "vmovaps 96(%1), %%ymm0;"
	    "vmovaps 96(%2), %%ymm0;"
	    "vmovaps 96(%3), %%ymm0;"
	    "vmovaps %%ymm0, 96(%0);"
#else
	    "movaps (%1), %%xmm0;"
	    "movaps (%2), %%xmm0;"
	    "movaps (%3), %%xmm0;"
	    "movaps %%xmm0, (%0);"

	    "movaps 16(%1), %%xmm0;"
	    "movaps 16(%2), %%xmm0;"
	    "movaps 16(%3), %%xmm0;"
	    "movaps %%xmm0, 16(%0);"

	    "movaps 32(%1), %%xmm0;"
	    "movaps 32(%2), %%xmm0;"
	    "movaps 32(%3), %%xmm0;"
	    "movaps %%xmm0, 32(%0);"

	    "movaps 48(%1), %%xmm0;"
	    "movaps 48(%2), %%xmm0;"
	    "movaps 48(%3), %%xmm0;"
	    "movaps %%xmm0, 48(%0);"

	    "movaps 64(%1), %%xmm0;"
	    "movaps 64(%2), %%xmm0;"
	    "movaps 64(%3), %%xmm0;"
	    "movaps %%xmm0, 64(%0);"

	    "movaps 80(%1), %%xmm0;"
	    "movaps 80(%2), %%xmm0;"
	    "movaps 80(%3), %%xmm0;"
	    "movaps %%xmm0, 80(%0);"

	    "movaps 96(%1), %%xmm0;"
	    "movaps 96(%2), %%xmm0;"
	    "movaps 96(%3), %%xmm0;"
	    "movaps %%xmm0, 96(%0);"

	    "movaps 112(%1), %%xmm0;"
	    "movaps 112(%2), %%xmm0;"
	    "movaps 112(%3), %%xmm0;"
	    "movaps %%xmm0, 112(%0);"
#endif
	    "add $128, %0;"
	    "add $128, %1;"
	    "add $128, %2;"
	    "add $128, %3;"
	    "sub $128, %4;"
	    "jnz _loop;"
#ifndef	USE_MIC
	    "mfence;"
#endif
	    : "=r" (unused0), "=r" (unused1), "=r" (unused2), "=r" (unused3),
          "=r" (unused4)
	    : "0" (dest->stream), "1"(src1->stream),
	      "2"(src2->stream), "3" (src3->stream), "4" (src3->size)
#ifdef USE_MIC
            : "%zmm0"
#elif (defined USE_AVX)
	    : "%ymm0"
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
