//
// Created by Jonas Bennink bolt on 11/4/22.
//

#ifndef FT_MALLOC_FT_MALLOC_H
#define FT_MALLOC_FT_MALLOC_H

#include <stddef.h>

void* ft_malloc(size_t size);
void* ft_realloc(void* ptr, size_t size);
void ft_free(void* ptr);

#endif //FT_MALLOC_FT_MALLOC_H
