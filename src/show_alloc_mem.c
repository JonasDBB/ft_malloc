#include "ft_malloc_private.h"
#include "ft_clib.h"
#include <pthread.h>

extern pthread_mutex_t g_lock;
extern heaps_t g_heaps;


static void write_ptr(void* ptr, size_t mem_size) {
    write(2, "0x", 2);
    unsigned long begin = (unsigned long)ptr;
    unsigned long end = (unsigned long)(ptr + mem_size);
    ft_write_nr_base(2, begin, 16);
    write(2, " - ", 3);
    ft_write_nr_base(2, end, 16);
    write(2, " : ", 3);
    ft_write_nr_base(2, mem_size, 10);
    write(2, " bytes\n", 7);
}

static size_t list_allocs(mem_region_t* list, size_t heap_size, size_t mem_size) {
    size_t ret = 0;
    mem_region_t* current = list;
    while (current != NULL) {
        char* mem_block = current->allocations;
        while ((void*)mem_block < (void*)current + heap_size) {
            if (*mem_block != FREE) {
//                write_ptr(mem_block + 1, mem_size);
                ret += mem_size;
            }
            mem_block += mem_size + 1;
        }
        current = current->next;
    }
    return ret;
}

static size_t list_large_allocs() {
    size_t ret = 0;
    mem_region_t* current = g_heaps.large;
    while (current != NULL) {
        size_t memsize = *(unsigned int*)current->allocations * getpagesize() - sizeof(unsigned int) - sizeof(mem_region_t);
        write_ptr(current->allocations + sizeof(unsigned int), memsize);
        ret += memsize;
        current = current->next;
    }
    return ret;
}

void show_alloc_mem() {
    size_t total = 0;
    write(2, "TINY :\n", 7);
    pthread_mutex_lock(&g_lock);
    total += list_allocs(g_heaps.tiny, TINY_HEAP_SIZE, TINY_MEM_SIZE);
    write(2, "SMALL :\n", 8);
    total += list_allocs(g_heaps.small, SMALL_HEAP_SIZE, SMALL_MEM_SIZE);
    write(2, "LARGE :\n", 8);
    total += list_large_allocs();
    pthread_mutex_unlock(&g_lock);
    write(2, "TOTAL : ", 8);
    ft_write_nr_base(2, total, 10);
    write(2, "\n", 1);
}
