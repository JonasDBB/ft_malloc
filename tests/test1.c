#include <stdlib.h>

int      main(void)
{
    int   i;
    char  *addr[1024];

    i = 0;
    while (i < 1024)
    {
        addr[i] = (char*)malloc(1024);
        addr[i][0] = 42;
        i++;
    }
    return (0);
}