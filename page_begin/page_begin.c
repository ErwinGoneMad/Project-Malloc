#include "page_begin.h"

void *page_begin(void *ptr, size_t page_size)
{
    union
    {
        void *ptr;
        size_t addr;
    } u;

    u.ptr = ptr;
    size_t mask = ~(page_size - 1);
    u.addr &= mask;
    return u.ptr;
}
