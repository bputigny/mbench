#include <config.h>

#include <symbol.h>

extern stream_tab_t *stream_tab;
extern run_tab_t *run_tab;
extern FILE *outfile;

void generate_run (run_tab_t *run_table);

void generate_header () {
    fprintf (outfile, "#include <stdio.h>\n");
    fprintf (outfile, "#include <stdlib.h>\n");
    fprintf (outfile, "#include <stdint.h>\n");
    fprintf (outfile, "#include <malloc.h>\n");
    fprintf (outfile, "#include <omp.h>\n");
    fprintf (outfile, "#include <bench.h>\n");
    fprintf (outfile, "\n");
    fprintf (outfile, "#include <libmbench.h>\n");
#ifdef USE_PAPI
    fprintf (outfile, "#include <papi.h>\n");
    fprintf (outfile, "#include <pthread.h>\n");
#endif
    fprintf (outfile, "\n");
}

void generate_declaration (stream_tab_t *stream_tab) {
    int i;
    for (i=0; i<stream_tab->used_size; i++) {
	fprintf (outfile, "stream_t *%s;\n", stream_tab->table[i].id);
    }
    fprintf (outfile, "uint64_t mbench_size = 0;\n");
    fprintf (outfile, "run_t *run_table = NULL;\n");
    fprintf (outfile, "\n");
}

void generate_stream_init (stream_tab_t *stream_tab) {
    int i;
    for (i=0; i<stream_tab->used_size; i++) {
	if (stream_tab->table[i].type == STATIC) {
	    fprintf (outfile, "    %s = mbench_stream_new (%lu, sysconf(_SC_PAGESIZE));\n",
		     stream_tab->table[i].id,
		     stream_tab->table[i].byte_size);
	}
	else {
	    fprintf (outfile, "    if (%d < mbench_num_stream_alignment) {\n", i);
	    fprintf (outfile, "        %s = mbench_stream_new (%s, mbench_stream_alignment[%d]);\n",
		     stream_tab->table[i].id,
		     "mbench_size",
		     i);
	    fprintf (outfile, "    }\n");
	    fprintf (outfile, "    else {\n");
	    fprintf (outfile, "        %s = mbench_stream_new (%s, sysconf(_SC_PAGESIZE));\n",
		     stream_tab->table[i].id,
		     "mbench_size");
	    fprintf (outfile, "    }\n");
	}
    }
}

void generate_cache_flush (stream_tab_t *stream_tab) {
    int i;
    for (i=0; i<stream_tab->used_size; i++) {
	fprintf (outfile, "            mbench_flush_stream (%s);\n", stream_tab->table[i].id);
    }
}

void generate_stream_free (stream_tab_t *stream_tab) {
    int i;
    
    for (i=0; i<stream_tab->used_size; i++) {
	fprintf (outfile, "    mbench_stream_free (%s);\n",
		 stream_tab->table[i].id);
    }
}

void write_binding (binding_t binding) {
    int j;
    fprintf (outfile, "            if (");
    switch(binding.type) {
    case LIST:
	for (j=0; j<binding.num_table->used_size; j++) {
	    fprintf (outfile, "(omp_id == %d)",
		     binding.num_table->table[j]);
	    if (j < binding.num_table->used_size - 1)
		fprintf (outfile, "||");
	}
	break;
    case RANGE:
	for (j=binding.start; j<=binding.stop; j++) {
	    fprintf (outfile, "(omp_id == %d)",
		     j);
	    if (j < binding.stop)
		fprintf (outfile, "||");
	}
	break;
    }
    fprintf (outfile, ") ");
}


void write_function_call (func_t func) {
    fprintf (outfile, " {\n");
    fprintf (outfile, "tmp = mbench_%s(%s);\n",
	     func.name,
	     func.arg);
    fprintf (outfile, " p.bytes += tmp.bytes;\n");
    fprintf (outfile, " p.instructions += tmp.instructions;\n");
    fprintf (outfile, " }\n");
}

void write_run (run_t run) {
    if (run.timer) {
#ifdef USE_PAPI
	fprintf (outfile, "    mbench_start_hwcounters ();\n");
#endif
	fprintf (outfile, "            tmp.bytes = 0;\n");
	fprintf (outfile, "            tmp.instructions = 0;\n");
	fprintf (outfile, "            #pragma omp barrier\n");
	fprintf (outfile, "            TIMER_START;\n");
	generate_run (run.l);
	fprintf (outfile, "            TIMER_STOP;\n");
#ifdef USE_PAPI
	fprintf (outfile, "    mbench_stop_hwcounters (omp_id, r);\n");
#endif
	fprintf (outfile, "            #pragma omp barrier\n");
	fprintf (outfile, "            if ((p.bytes > 0)) {\n");
	fprintf (outfile, "                #pragma omp critical\n");
	fprintf (outfile, "                {\n");
	fprintf (outfile, "                    cur += t;\n");
	fprintf (outfile, "                }\n");
	fprintf (outfile, "            }\n");
	fprintf (outfile, "            #pragma omp barrier\n");
    }
    else {
	write_binding (run.binding);
	write_function_call (run.func);
    }
}

void generate_run (run_tab_t *run_table) {
    int i;
    for (i=0; i<run_table->used_size; i++) {
	write_run (run_table->table[i]);
    }
}

void generate_print_perf (run_tab_t *run_table) {
    int i;
    for (i=0; i<run_table->used_size; i++) {
    	if (run_table->table[i].timer) {
	    generate_print_perf (run_table->table[i].l);
	    printf ("CALL\n");
	}
	else {
	    fprintf (outfile,
		     "    mbench_add_run (run_table, p, t, r, omp_id);\n");
	}
    }
    free (run_table->table);
    free (run_table);
}

void generate_main (stream_tab_t *stream_tab, run_tab_t *run_tab) {
    fprintf (outfile, "int main (int argc, char *argv[]) {\n");
    fprintf (outfile, "    mbench_parse_args (argc, argv);\n");
    fprintf (outfile, "    uint64_t *runtime_cycles;\n");
    fprintf (outfile, "    perf_t *perf;\n");
    fprintf (outfile, "    uint64_t cur;\n");
    fprintf (outfile, "    int nthreads, r;\n");

    fprintf (outfile, "    BEGIN_SIZE_LOOP\n");

    fprintf (outfile, "    #pragma omp parallel private(r)\n");
    fprintf (outfile, "    {\n");
    fprintf (outfile, "        #pragma omp master\n");
    fprintf (outfile, "        {\n");
    fprintf (outfile, "        nthreads = omp_get_num_threads ();\n");
    fprintf (outfile, "        runtime_cycles = \n");
    fprintf (outfile, "            malloc (nthreads * sizeof *runtime_cycles);\n");
    fprintf (outfile, "        perf = \n");
    fprintf (outfile, "            malloc (nthreads * sizeof *perf);\n");

#ifdef USE_PAPI
    fprintf (outfile, "    mbench_init_hwcounters (nthreads);\n");
#endif

    fprintf (outfile, "        run_table = \n");
    fprintf (outfile, "            calloc (REPS * nthreads, sizeof *run_table);\n");
    fprintf (outfile, "        }\n");

#ifdef USE_PAPI
    fprintf (outfile, "    if (PAPI_thread_init (pthread_self) != PAPI_OK) {fprintf (stderr, \"PAPI thread init failed\\n\"); exit (EXIT_FAILURE);}\n");
#endif

    fprintf (outfile, "        for (r=0; r<REPS; r++) {\n");
    fprintf (outfile, "            cur = 0;\n");
    fprintf (outfile, "            perf_t tmp = {0, 0};\n");

    fprintf (outfile, "        #pragma omp master\n");
    fprintf (outfile, "        {\n");
    generate_stream_init (stream_tab);
    fprintf (outfile, "        cur = 0;\n");
    fprintf (outfile, "        }\n");
    fprintf (outfile, "            #pragma omp barrier\n");
    generate_cache_flush (stream_tab);

    fprintf (outfile, "            uint64_t t = 0;\n");
    fprintf (outfile, "            perf_t p = {0, 0};\n");
    fprintf (outfile, "            int omp_id = omp_get_thread_num();\n");

    fprintf (outfile, "            #pragma omp barrier\n");
    generate_run (run_tab);
    fprintf (outfile, "        #pragma omp master\n");
    fprintf (outfile, "        {\n");
    generate_stream_free (stream_tab);
    fprintf (outfile, "        }\n");
    
    fprintf (outfile,
	     "    mbench_add_run (run_table, p, t, r, omp_id);\n");
    fprintf (outfile, "        #pragma omp barrier\n");
    fprintf (outfile, "        }\n");
    fprintf (outfile, "    }\n");

    fprintf (outfile, "    mbench_print_results (run_table, nthreads);\n");

    fprintf (outfile, "    free (runtime_cycles); free (perf);\n");
    fprintf (outfile, "    free (run_table);\n");

    fprintf (outfile, "    END_SIZE_LOOP\n");

    fprintf (outfile, "    mbench_cleanup (nthreads);\n");
    fprintf (outfile, "    return EXIT_SUCCESS;\n");
    fprintf (outfile, "}\n");
}

void code_generator () {
    generate_header ();
    generate_declaration (stream_tab);
    generate_main (stream_tab, run_tab);
}

