#include <bench.h>


perf_t mbench_ls (stream_t *src1, stream_t *dest) {
    perf_t ret = {0, 0};

    if (dest->size >= 128) {
	ret.instructions = 2*dest->size / 16;
	ret.bytes = 2*dest->size;
	__asm__ __volatile__(
	    "mfence;"
	    "_loop:"
#ifdef USE_AVX
	    "vmovaps (%1), %%ymm0;"
	    "vmovaps %%ymm0, (%0);"

	    "vmovaps 32(%1), %%ymm0;"
	    "vmovaps %%ymm0, 32(%0);"

	    "vmovaps 64(%1), %%ymm0;"
	    "vmovaps %%ymm0, 64(%0);"

	    "vmovaps 96(%1), %%ymm0;"
	    "vmovaps %%ymm0, 96(%0);"
#else
	    "movaps (%1), %%xmm0;"
	    "movaps %%xmm0, (%0);"

	    "movaps 16(%1), %%xmm0;"
	    "movaps %%xmm0, 16(%0);"

	    "movaps 32(%1), %%xmm0;"
	    "movaps %%xmm0, 32(%0);"

	    "movaps 48(%1), %%xmm0;"
	    "movaps %%xmm0, 48(%0);"

	    "movaps 64(%1), %%xmm0;"
	    "movaps %%xmm0, 64(%0);"

	    "movaps 80(%1), %%xmm0;"
	    "movaps %%xmm0, 80(%0);"

	    "movaps 96(%1), %%xmm0;"
	    "movaps %%xmm0, 96(%0);"

	    "movaps 112(%1), %%xmm0;"
	    "movaps %%xmm0, 112(%0);"
#endif

	    "add $128, %0;"
	    "add $128, %1;"
	    "sub $128, %2;"
	    "jnz _loop;"
	    "mfence;"
	    :
	    : "r" (dest->stream), "r"(src1->stream),
	      "r" (src1->size)
#ifdef USE_AVX
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
