#include <unistd.h>
#include "ft_clib.h"
#include "ft_malloc.h"
#include "ft_malloc_private.h"
#include <pthread.h>
#include <sys/mman.h>
#include <sys/resource.h>

pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

heaps_t g_heaps = {.tiny = NULL,
        .small = NULL,
        .large =NULL};

static mem_region_t* create_region(size_t size) {
    mem_region_t* new_region = (mem_region_t*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (new_region == MAP_FAILED) {
        write(2, "MAP ERROR\n", 10);
    }
    ft_bzero(new_region, size);
    new_region->allocations = (void*)new_region + sizeof(mem_region_t);
    return new_region;
}

static void* find_free_mem(mem_region_t* list, size_t heap_size, size_t mem_size) {
    if (list == NULL) {
        return NULL;
    }
    mem_region_t* curr_region = list;
    while (curr_region) {
        char* mem_block = curr_region->allocations;
        while ((void*)mem_block < (void*)curr_region + heap_size) {
            if (*mem_block == FREE) {
                *mem_block = ALLOC;
                return (mem_block + 1);
            }
            mem_block += mem_size + 1;
        }
        curr_region = curr_region->next;
    }
    return NULL;
}

static void append_heap(mem_region_t* new_heap, mem_region_t** list) {
    if (*list == NULL) {
        *list = new_heap;
        return;
    }
    mem_region_t* tmp = *list;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new_heap;
}

void* malloc_tiny_small(mem_region_t** list, size_t heap_size, size_t mem_size) {
    void* ret = find_free_mem(*list, heap_size, mem_size);
    while (ret == NULL) {
        append_heap(create_region(heap_size), list);
        ret = find_free_mem(*list, heap_size, mem_size);
    }
    return ret;
}

void* malloc_large(size_t size) {
    // extra size needed to keep track of allocation
    size += sizeof(mem_region_t) + sizeof(unsigned int);

    struct rlimit rlp;
    if (getrlimit(RLIMIT_DATA, &rlp) != 0) {
        return NULL;
    }
    if (rlp.rlim_max > size) {
        return NULL;
    }

    // for large allocations size should be multiple of getpagesize()
    size = (size + (getpagesize() - 1)) & ~(getpagesize() - 1);
    mem_region_t* new_region = create_region(size);
    append_heap(new_region, &g_heaps.large);
    // save nr of pages used. this only works up to unsigned int max * pages
    *(unsigned int*)new_region->allocations = size / getpagesize();
    return new_region->allocations + sizeof(unsigned int);
}

void* locked_malloc(size_t size) {
    if (size <= TINY_MEM_SIZE) {
        return malloc_tiny_small(&g_heaps.tiny, TINY_HEAP_SIZE, TINY_MEM_SIZE);
    } else if (size <= SMALL_MEM_SIZE) {
        return malloc_tiny_small(&g_heaps.small, SMALL_HEAP_SIZE, SMALL_MEM_SIZE);
    } else {
        return malloc_large(size);
    }
}

void* ft_malloc(size_t size) {
    pthread_mutex_lock(&g_lock);
    void* ret = locked_malloc(size);
    pthread_mutex_unlock(&g_lock);
    return ret;
}
