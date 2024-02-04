#include "tools.h"

#include <stdlib.h>

struct recycler *recycler_create(size_t block_size, size_t total_size)
{
    if (block_size == 0 || block_size % sizeof(struct free_list) != 0)
    {
        return NULL;
    }
    if (total_size == 0 || total_size % block_size != 0)
    {
        return NULL;
    }

    struct recycler *res = malloc(sizeof(struct recycler));
    if (!res)
        return NULL;

    res->block_size = block_size;
    res->capacity = total_size / block_size;
    res->chunk = malloc(total_size);

    if (!res->chunk)
    {
        free(res);
        return NULL;
    }

    struct free_list *curr = res->chunk;
    for (size_t i = 0; i < res->capacity - 1; i++)
    {
        struct free_list *next = curr + (block_size / sizeof(struct free_list));
        curr->next = next;
        curr = next;
    }
    curr->next = NULL;

    res->free = res->chunk;

    return res;
}

void recycler_destroy(struct recycler *r)
{
    if (!r)
        return;

    free(r->chunk);
    free(r);
}

void *recycler_allocate(struct recycler *r)
{
    if (!r)
        return NULL;

    if (r->free == NULL)
        return NULL;

    struct free_list *res = r->free;
    r->free = res->next;

    return res;
}

void recycler_free(struct recycler *r, void *block)
{
    if (!r || !block)
        return;

    struct free_list *head = block;
    head->next = r->free;
    r->free = block;
}
