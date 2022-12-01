#include <stdio.h>
#include <unistd.h>
#include "ft_clib.h"
//#include "ft_list.h"
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


tiny_mem_region_t* create_region() {
    tiny_mem_region_t* new_region = (tiny_mem_region_t*)mmap(NULL, TINY_HEAP_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (new_region == MAP_FAILED) {
        write(2, "MAP ERROR\n", 10);
    }
    ft_bzero(new_region, TINY_HEAP_SIZE);
    new_region->allocations = new_region;
    new_region->allocations += sizeof(tiny_mem_region_t);
    LOG("%p %p", new_region, new_region->allocations);
    LOG("diff %lu", (void*)new_region->allocations - (void*)new_region);
    LOG("array size is %lu", (TINY_HEAP_SIZE - sizeof(tiny_mem_region_t)) / (TINY_MEM_SIZE + 1));
    return new_region;
}

void* find_free_mem() {
    tiny_mem_region_t* curr_region = g_heaps.tiny;
    while (curr_region) {
        // loop array
        char* mem_block = curr_region->allocations;
        int i = 0;
        while ((void*)mem_block < (void*)curr_region + TINY_HEAP_SIZE) {
            if (*mem_block == FREE) {
                LOG("free block at %d", i);
                *mem_block = ALLOC;
                return (mem_block + 1);
            }
            ++i;
            mem_block += TINY_MEM_SIZE + 1;
        }
        curr_region = curr_region->next;
    }
    return NULL;
}

void append_tiny_heap(tiny_mem_region_t* new_heap) {
    tiny_mem_region_t* tmp = g_heaps.tiny;
    while (tmp->next) {
        tmp = tmp->next;
    }
    tmp->next = new_heap;
}

void* malloc_tiny() {
    if (g_heaps.tiny == NULL) {
        g_heaps.tiny = create_region();
    }
    void* ret = find_free_mem();
    while (ret == NULL) {
        append_tiny_heap(create_region());
        ret = find_free_mem();

    }
    return ret;
}

void* ft_malloc(size_t size) {
    if (size <= TINY_MEM_SIZE) {
        return malloc_tiny();
    } else {
        LOG("size not yet handled");
    }
    errno = ENOMEM;
    return NULL;
}

int main() {
    print_os_name();
    LOG("page size %lu", TINY_HEAP_SIZE);
    LOG("size %lu", sizeof(tiny_mem_region_t));
    for (int i = 0; i < 500; ++i) {
        LOG("%p", ft_malloc(16));
        LOG("%p", ft_malloc(25));
    }
    return(0);
}
