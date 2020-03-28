#include <string.h>
#include "bl.h"

#define ALLOC_INC_COUNT 127

bl_t *bl_new(size_t alloc_count, size_t el_size)
{
  bl_t *list = malloc(sizeof *list + alloc_count * el_size);

  if (list == NULL)
    return NULL;

  list->count = 0;
  list->el_size = el_size;
  list->alloc_count = alloc_count;
  return list;
}

bl_t *bl_dup(bl_t *list)
{
  bl_t *dup = bl_new(list->alloc_count, list->el_size);

  if (dup == NULL)
    return NULL;

  dup->count = list->count;
  memcpy(dup, list, sizeof *list + list->count * list->el_size);
  return dup;
}

void bl_del(bl_t *list)
{
  free(list);
}

void *bl_head(bl_t *list)
{
  if (!bl_count(list))
    return NULL;

  return list->data;
}

void *bl_tail(bl_t *list)
{
  if (!bl_count(list))
    return NULL;

  return list->data + (list->count - 1) * list->el_size;
}

bool bl_add(bl_t **list, void *data)
{
  if (bl_count(*list) > (*list)->alloc_count - 1)
  {
    bl_t *swap = realloc(*list, sizeof **list + ((*list)->alloc_count + ALLOC_INC_COUNT) * (*list)->el_size);

    if (swap == NULL)
      return 0;

    *list = swap;
    (*list)->alloc_count += ALLOC_INC_COUNT;
  }

  memcpy((*list)->data + (*list)->count * (*list)->el_size, data, (*list)->el_size);
  (*list)->count++;
  return 1;
}


void bl_remove(bl_t *list, void *data)
{
  if (data == bl_tail(list));

  else if (data == bl_head(list) && bl_count(list) > 1)
    memcpy(data, bl_next(list, data), (bl_count(list) - 1) * list->el_size);

  else
  {
    size_t i = 0;

    for (char *n = bl_tail(list); n; n = bl_prev(list, n), i++)
      if (n == data)
      {
        memcpy(n, bl_next(list, n), i * list->el_size);
        break;
      }
  }

  list->count--;
}

void *bl_next(bl_t *list, void *data)
{
  if (data == bl_tail(list))
    return NULL;

  return (char *) data + list->el_size;
}

void *bl_prev(bl_t *list, void *data)
{
  if (data == bl_head(list))
    return NULL;

  return (char *) data - list->el_size;
}

void *bl_itr_head(bl_t *list, size_t n)
{
  if (!bl_count(list) || n > list->count - 1)
    return NULL;

  return (char *) bl_head(list) + (n * list->el_size);
}

void *bl_itr_tail(bl_t *list, size_t n)
{
  if (!bl_count(list) || n > list->count - 1)
    return NULL;

  return (char *) bl_tail(list) - n * list->el_size;
}

void bl_remove_tail(bl_t *list)
{
  if (bl_count(list))
    list->count--;
}

size_t bl_count(bl_t *list)
{
  return list->count;
}

void bl_clear(bl_t *list)
{
  list->count = 0;
}

void bl_for_each(bl_t *list, void (*callback)(void *, void *), void *userp)
{
  for (char *n = bl_head(list); n; n = bl_next(list, n))
    callback(n, userp);
}
