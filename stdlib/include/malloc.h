#ifndef DDRAIG_OS_MALLOC_H
#define DDRAIG_OS_MALLOC_H

#include <stdbool.h>
#include <stddef.h>

bool malloc_init(const void *base, const void *limit, const size_t heap_blocks, const size_t split_thresh, const size_t alignment);
void *malloc(size_t num);
void *calloc(size_t num, size_t size);
bool free(void *ptr);

size_t malloc_num_free();
size_t malloc_num_used();
size_t malloc_num_fresh();

bool malloc_check();


#endif
