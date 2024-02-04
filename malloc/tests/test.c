#include <stdio.h>
#include <stdlib.h>

#include "tools.h"

void *my_calloc(size_t nmb, size_t size)
{
    if (__builtin_mul_overflow(nmb, size, &size))
    {
        return NULL;
    }

    void *ptr = my_malloc(size);
    if (!ptr)
    {
        return NULL;
    }

    char *res = void_char(ptr);
    size_t n = bl_size(size);
    for (size_t i = 0; i < n; i++)
    {
        res[i] = 0;
    }
    return res;
}

int main(void)
{
    int *test = my_malloc(sizeof(int) * 4100);
    for (int i = 0; i < 4160; i++)
    {
        test[i] = i;
    }

    char *caca = my_malloc(2);
    for (int i = 0; i < 2; i++)
    {
        caca[i] = 'a';
    }

    printf("int: ");
    for (int i = 0; i < 100; i++)
    {
        printf("%d ", test[i]);
    }
    printf("%d ", test[4099]);
    printf("%d ", *(test + 3));
    printf("%d ", *(test + 4120));
    printf("\n");

    my_free(test);
    // printf("%d ", *(test + 3));
    printf("char: ");
    for (int i = 0; i < 2; i++)
    {
        printf("%c ", caca[i]);
    }
    printf("\n");

    int *calla = my_calloc(10, sizeof(int));
    for (int i = 0; i < 10; i++)
    {
        printf("%d ", calla[i]);
    }
    printf("\n");
}
