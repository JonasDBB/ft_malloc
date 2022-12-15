//
// Created by Jonas Bennink bolt on 11/4/22.
//

#ifndef FT_MALLOC_FT_MALLOC_H
#define FT_MALLOC_FT_MALLOC_H

#include <stddef.h>

void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);

/**
 * Originally requested sizes are not saved, so all sizes will be SMALL_MEM_SIZE for small allocations, TINY_MEM_SIZE
 * for tiny allocations and a multiple of getpagesize (the amount used) for large allocations
 */
void show_alloc_mem();

#endif //FT_MALLOC_FT_MALLOC_H
