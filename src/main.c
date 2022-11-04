#include <stdio.h>
#include <unistd.h>
#include "ft_clib.h"
#include "ft_list.h"

#include <sys/utsname.h>
void print_os_name() {
    struct utsname uts;
    uname(&uts);
    printf("system is %s\n", uts.sysname);
}

int main() {
    print_os_name();
    printf("page size %d\n", getpagesize());
    return 0;
}
