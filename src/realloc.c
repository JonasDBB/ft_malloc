#include "ft_malloc.h"
#include "ft_malloc_private.h"
#include "ft_clib.h"
#include <pthread.h>
#include <sys/mman.h>

extern pthread_mutex_t g_lock;
extern heaps_t g_heaps;

static mem_size find_allocated_size(void* ptr) {
    mem_region_t* current = g_heaps.tiny;
    while (current != NULL) {
        if (ptr > current->allocations && ptr < current->allocations + TINY_HEAP_SIZE) {
            if (is_allocated_ptr(ptr, current, TINY)) {
                return TINY;
            }
            return ERROR;
        }
        current = current->next;
    }
    current = g_heaps.small;
    while (current != NULL) {
        if (ptr > current->allocations && ptr < current->allocations + SMALL_HEAP_SIZE) {
            if (is_allocated_ptr(ptr, current, SMALL)) {
                return SMALL;
            }
            return ERROR;
        }
        current = current->next;
    }
    current = g_heaps.large;
    while (current != NULL) {
        if (ptr == current->allocations + sizeof(unsigned int)) {
            if (is_allocated_ptr(ptr, current, LARGE)) {
                return LARGE;
            }
            return ERROR;
        }
        current = current->next;
    }
    return ERROR;
}

static bool need_realloc(void* ptr, size_t size, mem_size mem_category) {
    size_t new_size = size + sizeof(mem_region_t) + sizeof(unsigned int);
    // current size is large, new size is also large
    if (mem_category == LARGE && new_size >= (size_t)getpagesize()) {
        unsigned int pages_used = *(unsigned int*)(ptr - sizeof(unsigned int));
        new_size = (new_size + (getpagesize() - 1)) & ~(getpagesize() - 1);
        if (new_size / getpagesize() == pages_used) {
            // same nr of pages needed
            return false;
        }
    }
    // tiny to tiny or small to small realloc
    if ((mem_category == TINY && size <= TINY_MEM_SIZE) ||
        (mem_category == SMALL && size > TINY_MEM_SIZE && size <= SMALL_MEM_SIZE)) {
        return false;
    }
    return true;
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

    if (need_realloc(ptr, size, mem_category) == false) {
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

    size_t old_size = *(unsigned int*)(ptr - sizeof(unsigned int)) * getpagesize();
    if (old_size > size + sizeof(mem_region_t) + sizeof(unsigned int)) {
        size = (size + (getpagesize() - 1)) & ~(getpagesize() - 1);
        void* owning_ptr = ptr - sizeof(unsigned int) - sizeof(mem_region_t);
        // owning ptr is page aligned
        // size is new size alligned to multiple of pagesize
        // old_size - size is size reduction (in multiple of pagesize)
        if (munmap(owning_ptr + size, old_size - size) != 0) {
            write(2, "munmap failed!\n", 15);
        }
        // change new nr of pages
        *(unsigned int*)(ptr - sizeof(unsigned int)) = size / getpagesize();
        return ptr;
    }

    void* new_large = malloc_large(size);
    size_t len_to_copy = mem_category == TINY ? TINY_MEM_SIZE : SMALL_MEM_SIZE;
    ft_memcpy(new_large, ptr, len_to_copy);
    locked_free(ptr);
    return new_large;
}

void* realloc(void* ptr, size_t size) {
    pthread_mutex_lock(&g_lock);
    void* ret = locked_realloc(ptr, size);
    pthread_mutex_unlock(&g_lock);
    return ret;
}