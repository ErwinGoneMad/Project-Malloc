#ifndef TOOLS_H
#define TOOLS_H

#define _GNU_SOURCE

#include <stddef.h>
#include <sys/mman.h>
#include <unistd.h>

struct bucket
{
    size_t block_size;
    size_t nb_blocks;
    struct free_list *free;
    struct bucket *next;
    struct bucket *prev;
};

struct free_list
{
    struct free_list *next;
};

struct bucket *bucket_new(size_t block_size, struct bucket *prev_bucket);
void *my_malloc(size_t size);
void my_free(void *ptr);
void *page_begin(void *ptr, size_t page_size);
char *void_char(void *p);
size_t bl_size(size_t size);
struct bucket *void_bucket(void *p);

#endif // TOOLS_H
