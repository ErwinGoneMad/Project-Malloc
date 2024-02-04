#include "alignment.h"

size_t align(size_t size)
{
    size_t tmp = sizeof(long double);
    size_t res = (size + tmp - 1) & ~(tmp - 1);

    if (res < size)
        return 0;

    return res;
}
