#include <stdio.h>
#include <unistd.h>
#include "ft_clib.h"
#include "ft_malloc.h"
#include <pthread.h>
#include <errno.h>
#include <sys/mman.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

#include <sys/utsname.h>
void print_os_name() {
    struct utsname uts;
    uname(&uts);
    printf("system is %s\n", uts.sysname);
}

heaps_t g_heaps = {.tiny = NULL,
                   .small = NULL,
                   .large =NULL};


mem_region_t* create_region(size_t size) {
    LOG("creating new region with size %lu", size);
    mem_region_t* new_region = (mem_region_t*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (new_region == MAP_FAILED) {
        write(2, "MAP ERROR\n", 10);
    }
    ft_bzero(new_region, size);
    new_region->allocations = (void*)new_region + sizeof(mem_region_t);
    return new_region;
}

void* find_free_mem(mem_region_t* list, size_t heap_size, size_t mem_size) {
    mem_region_t* curr_region = list;
    while (curr_region) {
        char* mem_block = curr_region->allocations;
        int i = 0;
        while ((void*)mem_block < (void*)curr_region + heap_size) {
            if (*mem_block == FREE) {
                LOG("free block at %d", i);
                *mem_block = ALLOC;
                return (mem_block + 1);
            }
            ++i;
            mem_block += mem_size + 1;
        }
        curr_region = curr_region->next;
    }
    return NULL;
}

void append_heap(mem_region_t* new_heap, mem_region_t* list) {
    mem_region_t* tmp = list;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new_heap;
}

void* malloc_tiny_small(mem_region_t** list, size_t heap_size, size_t mem_size) {
    if (*list == NULL) {
        *list = create_region(heap_size);
    }
    void* ret = find_free_mem(*list, heap_size, mem_size);
    while (ret == NULL) {
        append_heap(create_region(heap_size), *list);
        ret = find_free_mem(*list, heap_size, mem_size);
    }
    return ret;
}

void* ft_malloc(size_t size) {
    if (size <= TINY_MEM_SIZE) {
        return malloc_tiny_small(&g_heaps.tiny, TINY_HEAP_SIZE, TINY_MEM_SIZE);
    } else if (size <= SMALL_MEM_SIZE) {
        return malloc_tiny_small(&g_heaps.small, SMALL_HEAP_SIZE, SMALL_MEM_SIZE);
    } else {
        // for large allocations size should be multiple of getpagesize()
        size = (size + (getpagesize() - 1)) & ~(getpagesize() - 1);
    }
    errno = ENOMEM;
    return NULL;
}

int main() {
    print_os_name();
//    for (int i = 0; i < 510; ++i) {
//        LOG("%p", ft_malloc(16));
//        LOG("%p", ft_malloc(67));
//    }
    return(0);
}
