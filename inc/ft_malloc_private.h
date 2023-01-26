//
// Created by Jonas Bennink bolt on 12/9/22.
//

#ifndef FT_MALLOC_FT_MALLOC_PRIVATE_H
#define FT_MALLOC_FT_MALLOC_PRIVATE_H

#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>

#define TINY_MEM_SIZE 128UL
#define SMALL_MEM_SIZE 1024UL
#define TINY_HEAP_SIZE (8 * getpagesize())
#define SMALL_HEAP_SIZE (64 * getpagesize())

typedef struct mem_region_s {
    void* allocations;
    struct mem_region_s* next;
} mem_region_t;

typedef struct heaps_s {
    mem_region_t* tiny;
    mem_region_t* small;
    mem_region_t* large;
} heaps_t;

typedef enum free_state_e {
    FREE = 0,
    ALLOC = 1
} free_state;

typedef enum mem_size_e {
    TINY,
    SMALL,
    LARGE,
    ERROR
} mem_size;

void* malloc_tiny_small(mem_region_t** list, size_t heap_size, size_t mem_size);
void* malloc_large(size_t size);
void* locked_malloc(size_t size);
bool is_allocated_ptr(void* ptr, mem_region_t* region, mem_size mem_size);
void locked_free(void* ptr);

#endif //FT_MALLOC_FT_MALLOC_PRIVATE_H
