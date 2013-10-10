#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *arg;
} func_t;

typedef struct {
    int *table;
    int used_size;
    int full_size;
} num_tab_t;

typedef struct {
    enum {LIST, RANGE} type;
    int start;
    int stop;
    num_tab_t *num_table;
} binding_t;

typedef struct {
    char *id;
    uint64_t byte_size;
    enum {STATIC, DYNAMIC} type;
} stream_t;

typedef struct {
    stream_t *table;
    uint64_t used_size;
    uint64_t full_size;
} stream_tab_t;

typedef struct run_list {
    struct run *table;
    uint64_t used_size;
    uint64_t full_size;
} run_tab_t;

typedef struct run {
    func_t func;
    binding_t binding;
    int timer;
    struct run_list *l;
} run_t;


stream_tab_t *stream_table_new ();
void stream_table_add (stream_tab_t *table, stream_t stream);
run_tab_t *run_table_new ();
void run_table_add (run_tab_t *table, run_t stream);

num_tab_t *num_table_new ();
void num_table_add (num_tab_t *table, int n);

#endif // SYMBOL_H
