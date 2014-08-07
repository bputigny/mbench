#include <bench.h>

perf_t mbench_clflush(stream_t *s) {
	char *addr = s->stream;
	int i; 

    perf_t ret = {0, 0};

	if (s->stream) {
#ifndef USE_MIC
		__asm__ __volatile__("mfence;"::);
#endif
		for (i=0; i<s->size; i+=64) {
			__asm__ __volatile__("clflush (%%rax);":: "a" (&addr[i]));
		}
#ifndef USE_MIC
		__asm__ __volatile__("mfence;"::);
#endif
	}
	
    return ret;
}

