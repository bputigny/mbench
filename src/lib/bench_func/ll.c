#include <bench.h>


perf_t ll (stream_t *src1, stream_t *src2) {
    perf_t ret = {0, 0};

    if (src1->size >= 128) {
	ret.instructions = 2*src1->size / 16;
	ret.bytes = 2*src1->size;
	__asm__ __volatile__(
	    "mfence;"
	    "_loop:"
#ifdef USE_AVX
	    "vmovaps (%0), %%ymm0;"
	    "vmovaps (%1), %%ymm1;"

	    "vmovaps 32(%0), %%ymm0;"
	    "vmovaps 32(%1), %%ymm1;"

	    "vmovaps 64(%0), %%ymm0;"
	    "vmovaps 64(%1), %%ymm1;"

	    "vmovaps 96(%0), %%ymm0;"
	    "vmovaps 96(%1), %%ymm1;"
#else
	    "movaps (%0), %%xmm0;"
	    "movaps (%1), %%xmm0;"

	    "movaps 16(%0), %%xmm0;"
	    "movaps 16(%1), %%xmm0;"

	    "movaps 32(%0), %%xmm0;"
	    "movaps 32(%1), %%xmm0;"

	    "movaps 48(%0), %%xmm0;"
	    "movaps 48(%1), %%xmm0;"

	    "movaps 64(%0), %%xmm0;"
	    "movaps 64(%1), %%xmm0;"

	    "movaps 80(%0), %%xmm0;"
	    "movaps 80(%1), %%xmm0;"

	    "movaps 96(%0), %%xmm0;"
	    "movaps 96(%1), %%xmm0;"

	    "movaps 112(%0), %%xmm0;"
	    "movaps 112(%1), %%xmm0;"
#endif
	    "add $128, %0;"
	    "add $128, %1;"
	    "sub $128, %2;"
	    "jnz _loop;"
	    "mfence;"
	    :
	    : "r"(src1->stream),
	      "r"(src2->stream), "r" (src2->size)
#ifdef USE_AVX
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
