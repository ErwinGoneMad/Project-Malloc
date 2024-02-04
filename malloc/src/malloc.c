#include <stddef.h>

#include "tools.h"

struct bucket *buckets = NULL;

size_t bl_size(size_t size)
{
    size_t res = 16;
    while (size > res)
    {
        res += 16;
    }
    return res;
}

struct bucket *find_bucket(size_t size)
{
    struct bucket *b = buckets;
    while (b != NULL)
    {
        if (b->block_size == size)
        {
            if (b->free == NULL)
            {
                continue;
            }
            return b;
        }
        b = b->next;
    }
    struct bucket *new_bucket = bucket_new(size, b);
    if (new_bucket == NULL)
    {
        return NULL;
    }
    if (b != NULL)
    {
        b->next = new_bucket;
        new_bucket->prev = b;
    }
    else
    {
        buckets = new_bucket;
    }
    return new_bucket;
}

void *my_malloc(size_t size)
{
    if (size == 0)
    {
        return NULL;
    }

    size_t block_size = bl_size(size);

    struct bucket *b = find_bucket(block_size);
    if (b == NULL)
    {
        return NULL;
    }

    if (b->free == NULL)
    {
        return NULL;
    }

    struct free_list *block = b->free;
    b->free = block->next;
    b->nb_blocks += 1;

    return (void *)block;
}

void my_free(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }

    size_t page_size = sysconf(_SC_PAGESIZE);
    struct bucket *b = void_bucket(page_begin(ptr, page_size));

    struct free_list *free_block = ptr;
    free_block->next = b->free;
    b->free = free_block;

    b->nb_blocks -= 1;

    if (b->nb_blocks == 0)
    {
        if (b->prev)
        {
            b->prev->next = b->next;
        }
        if (b->next)
        {
            b->next->prev = b->prev;
        }
        if (buckets == b)
        {
            buckets = b->next;
        }

        munmap(b, page_size);
    }
}

__attribute__((visibility("default"))) void *malloc(size_t size)
{
    return my_malloc(size);
}

__attribute__((visibility("default"))) void free(void *ptr)
{
    my_free(ptr);
}

__attribute__((visibility("default"))) void *realloc(void *ptr, size_t size)
{
    if (size == 0)
    {
        my_free(ptr);
        return NULL;
    }

    char *res = my_malloc(size);
    char *tmp = void_char(ptr);
    if (ptr == NULL)
    {
        return res;
    }

    size_t page_size = sysconf(_SC_PAGESIZE);
    struct bucket *b = void_bucket((page_begin(ptr, page_size)));
    if (!b)
    {
        return NULL;
    }
    for (size_t i = 0; i < b->block_size; i++)
    {
        if (i >= size)
            break;
        res[i] = tmp[i];
    }
    my_free(ptr);
    return res;
}

__attribute__((visibility("default"))) void *calloc(size_t nmemb, size_t size)
{
    if (__builtin_mul_overflow(nmemb, size, &size))
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
