#include "tools.h"

#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

struct blk_allocator *blka_new(void)
{
    struct blk_allocator *res = malloc(sizeof(struct blk_allocator));
    if (!res)
        return NULL;

    res->meta = NULL;
    return res;
}

struct blk_meta *blka_alloc(struct blk_allocator *allocator, size_t size)
{
    long p_size = sysconf(_SC_PAGESIZE);
    size_t nb = (sizeof(struct blk_meta) + size + p_size - 1) / p_size;
    size_t allocation_size = nb * p_size;
    struct blk_meta *new_meta =
        mmap(NULL, allocation_size, PROT_READ | PROT_WRITE,
             MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (new_meta == MAP_FAILED)
    {
        return NULL;
    }

    new_meta->size = allocation_size - sizeof(struct blk_meta);
    new_meta->next = allocator->meta;
    new_meta->is_free = 1;
    allocator->meta = new_meta;

    return new_meta;
}

void blka_free(struct blk_meta *blk)
{
    munmap(blk, blk->size + sizeof(struct blk_meta));
}

void blka_pop(struct blk_allocator *blka)
{
    struct blk_meta *tmp = blka->meta;
    if (tmp == NULL)
    {
        return;
    }

    blka->meta = tmp->next;

    munmap(tmp, tmp->size + sizeof(struct blk_meta));
}

void blka_delete(struct blk_allocator *blka)
{
    struct blk_meta *curr = blka->meta;

    while (curr != NULL)
    {
        struct blk_meta *tmp = curr;
        curr = curr->next;
        blka_free(tmp);
    }

    free(blka);
}

void split_block(struct blk_meta *block, size_t size)
{
    size_t remaining_size = block->size - size - sizeof(struct blk_meta);

    if (remaining_size > sizeof(struct blk_meta))
    {
        struct blk_meta *new_block = (struct blk_meta *)((char *)block + sizeof(struct blk_meta) + size);
        new_block->size = remaining_size;
        new_block->next = block->next;
        new_block->is_free = 1;

        block->size = size;
        block->next = new_block;
    }
}

void coalesce_free_blocks(struct blk_meta *block)
{
    if (block->next && block->next->is_free)
    {
        block->size += sizeof(struct blk_meta) + block->next->size;
        block->next = block->next->next;
    }
}

