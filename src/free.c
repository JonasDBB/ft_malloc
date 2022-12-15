#include "ft_malloc.h"
#include "ft_malloc_private.h"
#include "ft_clib.h"
#include <sys/mman.h>
#include <errno.h>
#include <pthread.h>

extern pthread_mutex_t g_lock;
extern heaps_t g_heaps;

static mem_region_t* find_owning_heap(void* ptr, mem_size* mem_size) {
    mem_region_t* current = g_heaps.tiny;
    while (current != NULL) {
        if (ptr > current->allocations && ptr < current->allocations + TINY_HEAP_SIZE) {
            *mem_size = TINY;
            return current;
        }
        current = current->next;
    }
    current = g_heaps.small;
    while (current != NULL) {
        if (ptr > current->allocations && ptr < current->allocations + SMALL_HEAP_SIZE) {
            *mem_size = SMALL;
            return current;
        }
        current = current->next;
    }
    current = g_heaps.large;
    while (current != NULL) {
        if (ptr == current->allocations + sizeof(unsigned int)) {
            *mem_size = LARGE;
            return current;
        }
        current = current->next;
    }
    return NULL;
}

static bool is_region_empty(mem_region_t* region, size_t heap_size, size_t mem_size) {
    char* mem_block = region->allocations;
    while ((void*)mem_block < (void*)region + heap_size) {
        if (*mem_block != FREE) {
            return false;
        }
        mem_block += mem_size + 1;
    }
    return true;
}

static void remove_from_list(mem_region_t** list, mem_region_t* elem) {
    mem_region_t* current = *list;
    if (current == elem) {
        *list = elem->next;
        return;
    }
    while (current->next != elem) {
        current = current->next;
    }
    current->next = elem->next;
}

void locked_free(void* ptr) {
    if (ptr == NULL) {
        return;
    }
    mem_size owning_size;
    mem_region_t* owning_heap = find_owning_heap(ptr, &owning_size);
    if (owning_heap == NULL) {
        // pointer being freed was not allocated
        errno = EINVAL;
        return;
    }
    if (owning_size == TINY || owning_size == SMALL) {
        *(char*)(ptr - 1) = 0;
        size_t heap_size = owning_size == TINY ? TINY_HEAP_SIZE : SMALL_HEAP_SIZE;
        size_t mem_size = owning_size == TINY ? TINY_MEM_SIZE : SMALL_MEM_SIZE;
        if (!is_region_empty(owning_heap, heap_size, mem_size)) {
            return;
        }
        remove_from_list(owning_size == TINY ? &g_heaps.tiny : &g_heaps.small, owning_heap);
        if (munmap(owning_heap, heap_size) != 0) {
            write(2, "munmap failed!\n", 15);
        }
        return;
    }
    remove_from_list(&g_heaps.large, owning_heap);
    unsigned int page_n = *(unsigned int*)owning_heap->allocations;
    if (munmap(owning_heap, page_n * getpagesize()) != 0) {
        write(2, "munmap failed!\n", 15);
    }
}

void free(void* ptr) {
    pthread_mutex_lock(&g_lock);
    locked_free(ptr);
    pthread_mutex_unlock(&g_lock);
}
