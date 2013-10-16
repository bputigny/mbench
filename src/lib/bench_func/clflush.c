#include <bench.h>

perf_t mbench_clflush(stream_t *s) {
	char *addr = s->stream;
	int i; 

    perf_t ret = {0, 0};

	if (s->stream) {
		__asm__ __volatile__("mfence;"::);
		for (i=0; i<s->size; i+=16) {
			__asm__ __volatile__("clflush (%%rax);":: "a" (&addr[i]));
		}
		__asm__ __volatile__("mfence;"::);
	}
	
    return ret;
}

