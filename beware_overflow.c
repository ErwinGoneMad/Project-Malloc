#include "tools.h"

void *beware_overflow(void *ptr, size_t nmemb, size_t size)
{
    size_t res;
    if (__builtin_mul_overflow(nmemb, size, &res))
    {
        return NULL;
    }

    char *_ptr = ptr;
    return _ptr + res;
}
