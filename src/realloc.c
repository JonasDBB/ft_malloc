#include "ft_malloc.h"
#include "ft_malloc_private.h"
#include "ft_clib.h"
#include <pthread.h>

extern pthread_mutex_t g_lock;
extern heaps_t g_heaps;

static mem_size find_allocated_size(void* ptr) {
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
        if (ptr == current->allocations + sizeof(unsigned int)) {
            return LARGE;
        }
        current = current->next;
    }
    return ERROR;
}

static void* locked_realloc(void* ptr, size_t size) {
    if (ptr == NULL) {
        return locked_malloc(size);
    }
    if (size == 0) {
        locked_free(ptr);
        return locked_malloc(TINY_MEM_SIZE);
    }
    mem_size mem_category = find_allocated_size(ptr);
    if (mem_category == ERROR) {
        return NULL;
    }
    if (mem_category == LARGE) {
        unsigned int pages_used = *(unsigned int*)(ptr - sizeof(unsigned int));
        size_t new_size = size + sizeof(mem_region_t) + sizeof(unsigned int);
        new_size = (new_size + (getpagesize() - 1)) & ~(getpagesize() - 1);
        if (new_size / getpagesize() == pages_used) {
            return ptr;
        }
    }
    if ((mem_category == TINY && size <= TINY_MEM_SIZE) ||
        (mem_category == SMALL && size > TINY_MEM_SIZE && size <= SMALL_MEM_SIZE)) {
        return ptr;
    }

    if (size <= TINY_MEM_SIZE) {
        void* new_tiny = malloc_tiny_small(&g_heaps.tiny, TINY_HEAP_SIZE, TINY_MEM_SIZE);
        ft_memcpy(new_tiny, ptr, TINY_MEM_SIZE);
        locked_free(ptr);
        return new_tiny;
    }

    if (size <= SMALL_MEM_SIZE) {
        void* new_small = malloc_tiny_small(&g_heaps.small, SMALL_HEAP_SIZE, SMALL_MEM_SIZE);
        size_t len_to_copy = mem_category == TINY ? TINY_MEM_SIZE : SMALL_MEM_SIZE;
        ft_memcpy(new_small, ptr, len_to_copy);
        locked_free(ptr);
        return new_small;
    }

    void* new_large = malloc_large(size);
    size_t len_to_copy = mem_category == TINY ? TINY_MEM_SIZE : SMALL_MEM_SIZE;
    ft_memcpy(new_large, ptr, len_to_copy);
    // TODO: from larger large to smaller large should only free ending pages
    locked_free(ptr);
    return new_large;
}

void* ft_realloc(void* ptr, size_t size) {
    pthread_mutex_lock(&g_lock);
    void* ret = locked_realloc(ptr, size);
    pthread_mutex_unlock(&g_lock);
    return ret;
}