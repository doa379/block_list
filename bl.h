#ifndef BL_H
#define BL_H

#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>

struct bl_s;

typedef struct bl_s
{
  size_t count, el_size, alloc_count;
  struct bl_s **addr;
  char data[];
} bl_t;

bl_t *bl_new(size_t);
bl_t *bl_dup(bl_t *);
void bl_del(bl_t *);
void bl_clear(bl_t *);
void *bl_next(bl_t *, void *);
void *bl_prev(bl_t *, void *);
void *bl_head(bl_t *);
void *bl_at(bl_t *, ssize_t);
void *bl_tail(bl_t *);
size_t bl_count(bl_t *);
bool bl_add(bl_t **, void *);
void bl_remove(bl_t *, void *);
void bl_remove_tail(bl_t *);
void bl_for_each(bl_t *, void (*)(void *, void *), void *);
void bl_reverse(bl_t **, bl_t *);

#endif
