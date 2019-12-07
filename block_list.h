#ifndef BLOCK_LIST_H
#define BLOCK_LIST_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
  size_t count, el_size, alloc_count;
  char data[];
} block_list_t;

block_list_t *block_list_new(size_t, size_t);
block_list_t *block_list_replicate(block_list_t *);
void block_list_del(block_list_t *);
void block_list_clear(block_list_t *);
void *block_list_next(block_list_t *, void *);
void *block_list_prev(block_list_t *, void *);
void *block_list_head(block_list_t *);
void *block_list_itr_head(block_list_t *, size_t);
void *block_list_tail(block_list_t *);
void *block_list_itr_tail(block_list_t *, size_t);
size_t block_list_count(block_list_t *);
bool block_list_add(block_list_t **, void *);
void block_list_remove(block_list_t *, void *);
void block_list_remove_tail(block_list_t *);
void block_list_for_each(block_list_t *, void (*)(void *, void *), void *);

#endif
