#define _GNU_SOURCE
#include <stdio.h>

#include <omp.h>

#include <config.h>

#include <libmbench.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <bench.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#ifdef USE_PAPI
#include <papi.h>
#endif

#define STR_SIZE 100

#define END		0
#define CHAR		1
#define SIZE		2
#define BANDWIDTH	3
#define LATENCY		4



uint64_t mbench_size = 0;
uint64_t mbench_size_max = 0;
uint64_t mbench_size_range = 0;
size_t *mbench_stream_alignment = NULL;
int mbench_num_stream_alignment = 0;
static uint64_t cpu_freq = 0;


static char *filename = NULL;
static FILE **csv_fd;
static FILE **dat_fd;

#ifdef USE_PAPI
int papi_num_events = 0;
int *papi_events;
long long *papi_counters;
#endif


void mbench_usage(char *name) {
	fprintf(stderr,
			"Usage: %s -s <size> [-o output-prefix] [-a alignment] -h\n",
			name);
}

stream_t *mbench_stream_new (uint64_t size, size_t align) {
	stream_t *s;
	int i;

	if (align % 16) {
		fprintf (stderr, "align must be a multiple of 16\n");
		exit (EXIT_FAILURE);
	}
	s = malloc (sizeof (*s));
	if (!s) {
		fprintf (stderr, "Out of mem\n");
		exit (EXIT_FAILURE);
	}

	s->allocated = size + align;
	s->size = size;
	uint64_t huge_page_size = 2*1024*1024;
	if (s->allocated % huge_page_size) {
		s->allocated = ((s->allocated / huge_page_size) + 1) * huge_page_size;
	}
#ifndef MAP_HUGETLB
#define MAP_HUGETLB 0
#endif
	s->buffer = mmap (NULL, s->allocated,
			PROT_READ|PROT_WRITE ,
			MAP_PRIVATE|MAP_ANONYMOUS|MAP_HUGETLB,
			-1, 0);
	if (s->buffer == MAP_FAILED) {
		s->buffer = mmap (NULL, s->allocated,
				PROT_READ|PROT_WRITE ,
				MAP_PRIVATE|MAP_ANONYMOUS,
				-1, 0);
		if (s->buffer == MAP_FAILED) {
			perror ("mmap failed");
			exit (EXIT_FAILURE);
		}
	}
	s->stream = s->buffer + align;

	void **addr = (void**) s->stream;
	for (i=0; i<size / sizeof (uint64_t); i++) {
		*addr = s->stream;
		addr++;
	}
	return s;
}

void mbench_stream_free (stream_t *s) {
	if (munmap (s->buffer, s->allocated)) {
		perror ("munmap failed");
		exit (EXIT_FAILURE);
	}
	free (s);
}

inline uint64_t mbench_rdtsc () {
	uint64_t ret;
	__asm__ __volatile__(
			"rdtsc;"
			"shl $32, %%rdx;"
			"add %%rdx, %%rax;"
			: "=a" (ret)
			);
	return ret;
}

uint64_t mbench_get_cpu_freq () {
	uint64_t ts_start, ts_stop;
	struct timeval tv_start, tv_stop;
	struct timespec delay = { 0, 800000000 };

	ts_start = mbench_rdtsc ();
	gettimeofday (&tv_start, NULL);
	nanosleep (&delay, NULL);
	ts_stop = mbench_rdtsc ();
	gettimeofday (&tv_stop, NULL);

	uint64_t f = 1000000 * (ts_stop - ts_start) /
		(((uint64_t)tv_stop.tv_sec * 1000000 + tv_stop.tv_usec) -
		 ((uint64_t)tv_start.tv_sec * 1000000 + tv_start.tv_usec));
	cpu_freq = f;
	return f;
}


void mbench_parse_args (int argc, char *argv[]) {
	int c, i;
	int option_index;
	char hostname[20];
	struct option long_options[] = {
		{"size", 1, 0, 's'},
		{0, 0, 0, 0}
	};
	int o_option = 0;

#ifdef USE_PAPI
#define ARG_STR	"o:s:c:a:"
	if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
		fprintf (stderr, "PAPI inittialization failed\n");
		exit (EXIT_FAILURE);
	}
#else
#define ARG_STR	"o:s:a:"
#endif
	while ((c = getopt_long (argc, argv, ARG_STR, long_options, &option_index)) != -1) {
#undef ARG_STR
		switch (c) {
#ifdef USE_PAPI
			case 'c':
				{
					int event_code;
					if (PAPI_event_name_to_code (optarg, &event_code) == PAPI_OK) {
						papi_num_events++;
						papi_events = realloc (papi_events, papi_num_events * sizeof (*papi_events));
						if (!papi_events) {
							fprintf (stderr, "out of mem\n");
							exit (EXIT_FAILURE);
						}
						papi_events[papi_num_events-1] = event_code;
					} else {
						fprintf (stderr, "Couldn't not add event %s (event not found)\n", optarg);
					}
				}
				break;
#endif
			case 'o':
				o_option = 1;
				asprintf (&filename, "%s", optarg);
				break;
			case 's':
				mbench_size = atoi (optarg);
				if (strstr (optarg, "kb") || strstr (optarg, "kB") || 
						strstr (optarg, "Kb") || strstr (optarg, "KB")) {
					mbench_size *= 1024;
				} else {	
					if (strstr (optarg, "mb") || strstr (optarg, "mB") || 
							strstr (optarg, "Mb") || strstr (optarg, "MB")) {
						mbench_size *= 1024 * 1024;
					} else {	
						if (strstr (optarg, "gb") || strstr (optarg, "gB") || 
								strstr (optarg, "Gb") || strstr (optarg, "GB")) {
							mbench_size *= 1024 * 1024 * 1024;
						}
					}
				}
				if (strstr (optarg, ":")) {
					mbench_size_range = 1;
					optarg = strstr (optarg, ":") + 1;	
					mbench_size_max = atoi (optarg);
					if (strstr (optarg, "kb") || strstr (optarg, "kB") || 
							strstr (optarg, "Kb") || strstr (optarg, "KB")) {
						mbench_size_max *= 1024;
					} else {	
						if (strstr (optarg, "mb") || strstr (optarg, "mB") || 
								strstr (optarg, "Mb") || strstr (optarg, "MB")) {
							mbench_size_max *= 1024 * 1024;
						} else {
							if (strstr (optarg, "gb") || strstr (optarg, "gB") || 
									strstr (optarg, "Gb") || strstr (optarg, "GB")) {
								mbench_size_max *= 1024 * 1024 * 1024;
							}
						}
					}
				}
				else {
					mbench_size_max = mbench_size;
				}
				break;
			case 'a':
				mbench_num_stream_alignment++;
				mbench_stream_alignment = realloc (mbench_stream_alignment,
						mbench_num_stream_alignment*sizeof (*mbench_stream_alignment));
				mbench_stream_alignment[mbench_num_stream_alignment - 1] = atoi (optarg);
				break;
		}
	}
	if (!o_option) {
		gethostname(hostname, 20);

		asprintf (&filename, "output/%s/%s", hostname, argv[0]);
	}
	char *dir_name = strdup (filename);
	for (i=0; filename[i] != '\0'; i++) {
		if (filename[i] == '/') {
			strncpy (dir_name, filename, i+1);
			dir_name[i] = '\0';
			if (access (dir_name, R_OK)) {
				mkdir (dir_name, 0755);
			}
		}
	}
	free (dir_name);

	if (mbench_size <= 0) {
		mbench_usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	printf ("%s: %s\n", argv[0], filename);
	mbench_get_cpu_freq ();
}


void mbench_flush_stream (stream_t *s) {
	uint64_t i;
	char *addr = s->stream;

	if (s->stream) {
		__asm__ __volatile__("mfence;"::);
		for (i=0; i<s->size; i+=16) {
			__asm__ __volatile__("clflush (%0);"
					:
					: "r" (&addr[i]));
		}
		__asm__ __volatile__("mfence;"::);
	}
}


void mbench_add_run (run_t *tab, perf_t perf, uint64_t c, int rep, int th) {
	tab[th*REPS + rep].perf = perf;
	tab[th*REPS + rep].runtime = c;
}


void mbench_print_results (run_t *run_table, int nthreads) {
	int i, r;
	char *csv_filename = NULL;
	char *dat_filename = NULL;

	static int count = 0;

	if (count == 0) {
		csv_fd = malloc (nthreads * sizeof (*csv_fd));
		dat_fd = malloc (nthreads * sizeof (*dat_fd));
		for (i=0; i<nthreads; i++) {
			csv_fd[i] = NULL;
			dat_fd[i] = NULL;
		}
	}

	for (i=0; i<nthreads; i++) {
		uint64_t bytes = run_table[i*REPS].perf.bytes;
		uint64_t instructions = run_table[i*REPS].perf.instructions;
		double size_kb = (double) bytes / 1024;
		double best_bandwidth = 0;
		double best_latency = 0;
		if (bytes > 0 || instructions > 0) {
			if (count == 0) {
				asprintf (&csv_filename, "%s-th%d.csv", filename, i);
				csv_fd[i] = fopen (csv_filename, "w");
				if (!csv_fd[i]) {
					fprintf (stderr, "Error: Could not open %s: ", csv_filename);
					perror ("");
					exit (EXIT_FAILURE);
				}

				asprintf (&dat_filename, "%s-th%d.dat", filename, i);
				dat_fd[i] = fopen (dat_filename, "w");
				if (!dat_fd[i]) {
					fprintf (stderr, "Error: Could not open %s: ", dat_filename);
					perror ("");
					exit (EXIT_FAILURE);
				}

				fprintf (csv_fd[i], ";Bandwidth (GB/s)\n");
				fprintf (csv_fd[i], "Size (KB);");
				for (r=0; r<REPS; r++) {
					fprintf (csv_fd[i], ";");
				}
				fprintf (csv_fd[i], "BEST; STD DEV;\n");
#ifdef USE_PAPI
				int n;
				for (n=0; n<papi_num_events; n++) {
					char cpt_name[100];
					PAPI_event_code_to_name (papi_events[i], cpt_name);
					fprintf (csv_fd[i], "%s;", cpt_name);
				}
#endif
				fprintf (csv_fd[i], ";");
			}
			fprintf (csv_fd[i], "%.2f;", size_kb);
			int best_rep = 0;
			for (r=0; r<REPS; r++) {
				uint64_t cycles = run_table[i*REPS + r].runtime;
				uint64_t instructions = run_table[i*REPS + r].perf.instructions;

				double bandwidth = (double) bytes / 1e9 * cpu_freq / (cycles);
				double latency = (double) cycles / instructions;

				fprintf (csv_fd[i], "%.2lf;",
						bandwidth);

				if (best_bandwidth < bandwidth || r == 0) {
					best_bandwidth = bandwidth;
					best_latency = latency;
					best_rep = 1;
				}
			}
			fprintf (csv_fd[i], "=MAX(%c%d:%c%d);",
					'B', count+i+3,
					'B' + REPS-1, count+i+3);
			fprintf (csv_fd[i], "=STDEVP(%c%d:%c%d);\n",
					'B', count+i+3,
					'B' + REPS-1, count+i+3);
#ifdef USE_PAPI
			int n;
			for (n=0; n<papi_num_events; n++) {
				fprintf (csv_fd[i], "%ld;", papi_counters[(i * REPS + best_rep)*papi_num_events + n]);
			}
#endif
			fflush (csv_fd[i]);
			fprintf (dat_fd[i], "%.2f\t%.2f\t%.2f\n", size_kb, best_bandwidth, best_latency);
		}
		fflush (dat_fd[i]);
	}
	if (count == 0) {
		free (dat_filename);
		free (csv_filename);
	}
	count ++;
}


void mbench_cleanup (int nthreads) {
	int i;
	for (i=0; i<nthreads; i++) {
		if (csv_fd[i])
			fclose (csv_fd[i]);
		if (dat_fd[i])
			fclose (dat_fd[i]);
	}
	free (csv_fd);
	free (dat_fd);
	free (mbench_stream_alignment);
#ifdef USE_PAPI
	free (papi_counters);
	free (papi_events);
#endif
}



void mbench_progress_bar (float percent) {
	struct winsize w;
	int columns, i, bar_size;
	ioctl(0, TIOCGWINSZ, &w);
	columns = w.ws_col - 11;

	bar_size = percent * columns;
	printf ("|");
	for (i=0; i<bar_size-1; i++)
		printf ("=");
	if (bar_size == columns)
		printf ("=");
	else
		printf ("O");
	for (i=bar_size; i<columns; i++)
		printf ("─");
	printf ("| %5.2f%% ", 100*percent);
	fflush (stdout);
	printf ("\r");
}



#ifdef USE_PAPI
void mbench_init_hwcounters (int nthreads) {
	papi_counters = malloc (nthreads * REPS * papi_num_events * sizeof (*papi_counters));
	if (!papi_counters) {
		fprintf (stderr, "ERROR: couldn't allocate memory for PAPI counters\n");
		exit (EXIT_FAILURE);
	}

}
void mbench_start_hwcounters () {
	if (papi_num_events > 0) {
		int err_code = PAPI_start_counters (papi_events, papi_num_events);
		if (err_code != PAPI_OK) {
			fprintf (stderr, "ERROR: Couldn't start PAPI counters: %d\n", err_code);
			exit (EXIT_FAILURE);
		}
	}
}

void mbench_stop_hwcounters (int thread_id, int r) {
	if (papi_num_events > 0) {
		if (PAPI_stop_counters (&papi_counters[(thread_id * REPS + r)*papi_num_events], papi_num_events) != PAPI_OK) {
			fprintf (stderr, "ERROR: Couldn't stop PAPI counters\n");
			exit (EXIT_FAILURE);
		}
	}
}
#endif
