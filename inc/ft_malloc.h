//
// Created by Jonas Bennink bolt on 11/4/22.
//

#ifndef FT_MALLOC_FT_MALLOC_H
#define FT_MALLOC_FT_MALLOC_H
#include <stddef.h>
#include <stdbool.h>
#include <unistd.h>

#define TINY_MEM_SIZE 32UL
#define SMALL_MEM_SIZE 128UL
#define TINY_HEAP_SIZE (4 * getpagesize())
#define SMALL_HEAP_SIZE (16 * getpagesize())

typedef struct mem_region_s {
    void* allocations;
    struct mem_region_s* next;
} mem_region_t;

typedef struct heaps_s {
    mem_region_t* tiny;
    mem_region_t* small;
    mem_region_t* large;
} heaps_t;

enum free_state {
    FREE = 0,
    ALLOC = 1
};

#endif //FT_MALLOC_FT_MALLOC_H
