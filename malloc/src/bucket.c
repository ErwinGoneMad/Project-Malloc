#include "tools.h"

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

char *void_char(void *p)
{
    return (char *)p;
}

struct bucket *void_bucket(void *p)
{
    return (struct bucket *)p;
}

long get_page_size(size_t size)
{
    long page_size = sysconf(_SC_PAGESIZE);
    return (size + page_size - 1) & ~(page_size - 1);
}

void *align_ptr(void *ptr, size_t size)
{
    char *res = void_char(ptr);
    size_t tmp = sizeof(long double);
    size_t aligned = (size + tmp - 1) & ~(tmp - 1);

    if (aligned < size)
        return res;

    return res + aligned;
}

struct bucket *bucket_new(size_t block_size, struct bucket *prev_bucket)
{
    size_t meta_size = sizeof(struct bucket);
    long page_size = get_page_size(block_size + meta_size);
    void *ptr = mmap(NULL, page_size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED)
    {
        return NULL;
    }

    struct bucket *b = void_bucket(ptr);
    b->block_size = block_size;
    b->next = NULL;
    b->nb_blocks = 0;
    b->prev = prev_bucket;
    if (prev_bucket)
    {
        prev_bucket->next = b;
    }

    void *first_block = align_ptr(ptr, meta_size);
    b->free = (struct free_list *)first_block;

    char *p = first_block;
    struct free_list *current = (struct free_list *)p;
    for (size_t i = block_size; i < page_size - meta_size; i += block_size)
    {
        p += block_size;
        if (p + block_size > void_char(ptr) + page_size)
            break;
        current->next = (struct free_list *)p;
        current = current->next;
    }
    current->next = NULL;

    return b;
}
