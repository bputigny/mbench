#include <symbol.h>

stream_tab_t *stream_table_new () {
    stream_tab_t *table = malloc (sizeof (*table));
    if (!table) {
	fprintf (stderr, "ERROR: out of mem\n");
	exit (EXIT_FAILURE);
    }
    table->used_size = 0;
    table->table = malloc (sizeof (*table->table));
    if (!table->table) {
	fprintf (stderr, "ERROR: out of mem\n");
	exit (EXIT_FAILURE);
    }
    table->full_size = 1;
    return table;
}


void stream_table_add (stream_tab_t *table, stream_t stream) {
    if (!table || !(table->table)) {
	fprintf (stderr, "ERROR: table uninitialized\n");
	exit (EXIT_FAILURE);
    }
    if (table->used_size == table->full_size) {
	table->full_size *= 2;
	table->table =
	    realloc (table->table, table->full_size*sizeof(*table->table));
	if (!table->table) {
	    fprintf (stderr, "ERROR: out of mem\n");
	    exit (EXIT_FAILURE);
	}
    }
    table->table[table->used_size++] = stream;
}


run_tab_t *run_table_new () {
    run_tab_t *table = malloc (sizeof (*table));
    if (!table) {
	fprintf (stderr, "ERROR: out of mem\n");
	exit (EXIT_FAILURE);
    }
    table->used_size = 0;
    table->table = malloc (sizeof (*table->table));
    if (!table->table) {
	fprintf (stderr, "ERROR: out of mem\n");
	exit (EXIT_FAILURE);
    }
    table->full_size = 1;
    return table;
}

void run_table_add (run_tab_t *table, run_t stream) {
    if (!table || !(table->table)) {
	fprintf (stderr, "ERROR: table uninitialized\n");
	exit (EXIT_FAILURE);
    }
    if (table->used_size == table->full_size) {
	table->full_size *= 2;
	table->table =
	    realloc (table->table, table->full_size*sizeof(*table->table));
	if (!table->table) {
	    fprintf (stderr, "ERROR: out of mem\n");
	    exit (EXIT_FAILURE);
	}
    }
    table->table[table->used_size++] = stream;
}


void run_table_free (run_tab_t *table) {
    free (table->table);
    free (table);
}


num_tab_t *num_table_new () {
    num_tab_t *table = malloc (sizeof (*table));
    if (!table) {
	fprintf (stderr, "ERROR: out of mem\n");
	exit (EXIT_FAILURE);
    }
    table->used_size = 0;
    table->table = malloc (sizeof (*table->table));
    if (!table->table) {
	fprintf (stderr, "ERROR: out of mem\n");
	exit (EXIT_FAILURE);
    }
    table->full_size = 1;
    return table;
}


void num_table_add (num_tab_t *table, int n) {
    if (!table || !(table->table)) {
	fprintf (stderr, "ERROR: table uninitialized\n");
	exit (EXIT_FAILURE);
    }
    if (table->used_size == table->full_size) {
	table->full_size *= 2;
	table->table =
	    realloc (table->table, table->full_size*sizeof(*table->table));
	if (!table->table) {
	    fprintf (stderr, "ERROR: out of mem\n");
	    exit (EXIT_FAILURE);
	}
    }
    table->table[table->used_size++] = n;
}

