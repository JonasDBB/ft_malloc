#include "ft_malloc.h"
#include "ft_clib.h"
#include <sys/utsname.h>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

void print_os_name() {
    struct utsname uts;
    uname(&uts);
    fprintf(stderr, "system is %s\n", uts.sysname);
}

void leaks() {
    system("leaks ft_malloc");
}

void fun() {
    void* p = malloc(5000);
    void* q = malloc(500);
    void* r = malloc(14);
    void* s = malloc(12);
//    show_alloc_mem();
    free(r);
//    show_alloc_mem();
    free(p);
    free(q);
    free(s);
}

int main() {
    print_os_name();
    ft_write_nr_base(2, getpid(), 10);
    write(2, "\n", 1);
    int x = 4;
    ft_write_nr_base(2, x, 10);
    return (0);
//    rus();
//    while (1) {
//        void* p = malloc(9000);
//        p = realloc(p, 3000);
//        free(p);
//        free(p);
//    }
    char* p = (char*)malloc(10077);
    if (p == NULL) {
        write(2, "p is null\n", 10);
    } else {
        write(2, "p is NOT null\n", 14);
    }
    for (int i = 0; i < 20; ++i) {
        p[i] = 'a';
    }
    show_alloc_mem();
    p = realloc(p, 4075);
    ft_write_nr_base(2, errno, 10);
    show_alloc_mem();
    free(p);
//    atexit(leaks);
//    fun();
    return (0);
}
