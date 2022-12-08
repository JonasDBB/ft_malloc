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
    if (list == NULL) {
        return NULL;
    }
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

void append_heap(mem_region_t* new_heap, mem_region_t** list) {
//    LOG("%s", __func__);
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
//    LOG("%s", __func__);
    void* ret = find_free_mem(*list, heap_size, mem_size);
    while (ret == NULL) {
        append_heap(create_region(heap_size), list);
        ret = find_free_mem(*list, heap_size, mem_size);
    }
    return ret;
}

void* malloc_large(size_t size) {
    // for large allocations size should be multiple of getpagesize()
    size = (size + (getpagesize() - 1)) & ~(getpagesize() - 1);
    mem_region_t* new_region = create_region(size);
    append_heap(new_region, &g_heaps.large);
    return new_region->allocations;
}

mem_size find_allocated_size(void* ptr) {
    if (ptr == NULL) {
        return ERROR;
    }
    mem_region_t* current = g_heaps.tiny;
    while (current != NULL) {
        if (ptr > current->allocations && ptr < current->allocations + TINY_HEAP_SIZE) {
            return TINY;
        }
        current = current->next;
    }
    current = g_heaps.small;
    while (current != NULL) {
        if (ptr > current->allocations && ptr < current->allocations + SMALL_HEAP_SIZE) {
            return SMALL;
        }
        current = current->next;
    }
    current = g_heaps.large;
    while (current != NULL) {
        if (ptr == current->allocations) {
            return LARGE;
        }
        current = current->next;
    }
    return ERROR;
}

void* ft_realloc(void* ptr, size_t size) {
    mem_size mem_category = find_allocated_size(ptr);
    if (mem_category == ERROR) {
        return NULL;
    }
    if ((mem_category == TINY && size <= TINY_MEM_SIZE) ||
        (mem_category == SMALL && size > TINY_MEM_SIZE && size <= SMALL_MEM_SIZE)) {
        return ptr;
    }

    if (size <= TINY_MEM_SIZE) {
        void* new_tiny = malloc_tiny_small(&g_heaps.tiny, TINY_HEAP_SIZE, TINY_MEM_SIZE);
        ft_memcpy(new_tiny, ptr, TINY_MEM_SIZE);
        if (mem_category == LARGE) {
            // TODO: free large
        } else {
            *(char*)(ptr - 1) = 0;
        }
        return new_tiny;
    }

    if (size <= SMALL_MEM_SIZE) {
        void* new_small = malloc_tiny_small(&g_heaps.small, SMALL_HEAP_SIZE, SMALL_MEM_SIZE);
        size_t len_to_copy = mem_category == TINY ? TINY_MEM_SIZE : SMALL_MEM_SIZE;
        ft_memcpy(new_small, ptr, len_to_copy);
        if (mem_category == LARGE) {
            // TODO: free large
        } else {
            *(char*)(ptr - 1) = 0;
        }
        return new_small;
    }

    void* new_large = malloc_large(size);
    size_t len_to_copy = mem_category == TINY ? TINY_MEM_SIZE : SMALL_MEM_SIZE;
    ft_memcpy(new_large, ptr, len_to_copy);
    *(char*)(ptr - 1) = 0;
    return new_large;
}

void* ft_malloc(size_t size) {
    if (size <= TINY_MEM_SIZE) {
        return malloc_tiny_small(&g_heaps.tiny, TINY_HEAP_SIZE, TINY_MEM_SIZE);
    } else if (size <= SMALL_MEM_SIZE) {
        return malloc_tiny_small(&g_heaps.small, SMALL_HEAP_SIZE, SMALL_MEM_SIZE);
    } else {
        return malloc_large(size);
    }
}

int main() {
    print_os_name();
//    for (int i = 0; i < 510; ++i) {
//        LOG("%p", ft_malloc(16));
//        LOG("%p", ft_malloc(67));
//    }

    return(0);
}
