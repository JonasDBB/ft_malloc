#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void show_alloc_mem();

int main() {
    void* p = malloc(5000);
    p = realloc(p, 2000);
    p = realloc(p, 500);
    free(p);
    return (0);
}
