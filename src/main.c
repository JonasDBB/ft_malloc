#include "ft_malloc.h"
#include <sys/utsname.h>
#include <stdio.h>
#include "ft_clib.h"
#include <limits.h>
#include <unistd.h>

void print_os_name() {
    struct utsname uts;
    uname(&uts);
    fprintf(stderr, "system is %s\n", uts.sysname);
}

int main() {
    print_os_name();
//    for (int i = 0; i < 510; ++i) {
//        void *p = ft_malloc(16);
//        LOG("%p", p);
//        p = ft_realloc(p, 20);
//        LOG("%p", p);
//        p = ft_realloc(p, 50);
//        LOG("%p", p);
//        LOG("%p", ft_malloc(16));
//        LOG("%p", ft_malloc(67));
//    }
//    LOG("short max %lu", USHRT_MAX);
//    LOG("max memsize %lu", USHRT_MAX * getpagesize());
//    void* p = ft_malloc(5000);
//    LOG("%p", p);
//    p = ft_realloc(p, 7000);
//    LOG("%p", p);
//    p = ft_realloc(p, 2000);
//    LOG("%p", p);
//    p = ft_realloc(p, 20000);
//    LOG("%p", p);
//    p = ft_realloc(p, 19000);
//    LOG("%p", p);
    char* arr1[512];
    char* arr2[512];
    for (int i = 0; i < 512; ++i) {
        arr1[i] = ft_malloc(17);
        arr2[i] = ft_malloc(69);
    }
    for (int i = 0; i < 512; ++i) {
        ft_free(arr1[i]);
        ft_free(arr2[i]);
    }
    return (0);
}
