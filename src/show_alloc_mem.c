#include "ft_malloc_private.h"

extern heaps_t g_heaps;

static char x_digit(int c) {
    if (c < 10) {
        return (char)(c + '0');
    }
    return (char)('A' + c - 10);
}

static void write_nr(unsigned long n, unsigned int base) {
    if (n >= base) {
        write_nr(n / base, base);
    }
    char c = x_digit((int)(n % base));
    write(2, &c, 1);
}

static void write_ptr(void* ptr, size_t mem_size) {
    write(2, "0x", 2);
    unsigned long begin = (unsigned long)ptr;
    unsigned long end = (unsigned long)(ptr + mem_size);
    write_nr(begin, 16);
    write(2, " - ", 3);
    write_nr(end, 16);
    write(2, " : ", 3);
    write_nr(mem_size, 10);
    write(2, " bytes\n", 7);
}

static size_t list_allocs(mem_region_t* list, size_t heap_size, size_t mem_size) {
    size_t ret = 0;
    mem_region_t* current = list;
    while (current != NULL) {
        char* mem_block = current->allocations;
        while ((void*)mem_block < (void*)current + heap_size) {
            if (*mem_block != FREE) {
                write_ptr(mem_block + 1, mem_size);
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
        size_t memsize = *(unsigned int*)current->allocations * getpagesize();
        write_ptr(current->allocations + sizeof(unsigned int), memsize);
        ret += memsize;
        current = current->next;
    }
    return ret;
}

void show_alloc_mem() {
    size_t total = 0;
    write(2, "TINY :\n", 7);
    total += list_allocs(g_heaps.tiny, TINY_HEAP_SIZE, TINY_MEM_SIZE);
    write(2, "SMALL :\n", 8);
    total += list_allocs(g_heaps.small, SMALL_HEAP_SIZE, SMALL_MEM_SIZE);
    write(2, "LARGE :\n", 8);
    total += list_large_allocs();
    write(2, "TOTAL : ", 8);
    write_nr(total, 10);
    write(2, "\n", 1);
}
