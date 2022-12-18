//#include "ft_malloc.h"
//#include "ft_clib.h"
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

int main() {
    print_os_name();
//    while(1);
    void* p = malloc(2000);
//    show_alloc_mem();
//    ft_write_nr_base(2, getpagesize(), 10);
    p = realloc(p, 12);
//    show_alloc_mem();
    free(p);
//    show_alloc_mem();
    return (0);
}
