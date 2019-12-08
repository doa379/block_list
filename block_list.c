#include <string.h>
#include "block_list.h"

#define ALLOC_INC_COUNT 128

block_list_t *block_list_new(size_t alloc_count, size_t el_size)
{
  block_list_t *list = malloc(sizeof *list + alloc_count * el_size);
  
  if (list == NULL)
    return NULL;
  
  list->count = 0;
  list->el_size = el_size;
  list->alloc_count = alloc_count;
  return list;
}

block_list_t *block_list_replicate(block_list_t *list)
{
  block_list_t *replicate = block_list_new(list->alloc_count, list->el_size);

  if (replicate == NULL)
    return NULL;

  replicate->count = list->count;
  memcpy(replicate, list, sizeof *list + list->count * list->el_size);
  return replicate;
}

void block_list_del(block_list_t *list)
{
  free(list);
}

void *block_list_head(block_list_t *list)
{
  if (!block_list_count(list))
    return NULL;

  return list->data;
}

void *block_list_tail(block_list_t *list)
{
  if (!block_list_count(list))
    return NULL;

  return list->data + (list->count - 1) * list->el_size;
}

bool block_list_add(block_list_t **list, void *data)
{
  if (block_list_count(*list) > (*list)->alloc_count - 1)
    {
      block_list_t *swap = realloc(*list, sizeof **list + ((*list)->alloc_count + ALLOC_INC_COUNT) * (*list)->el_size);

      if (swap == NULL)
	return 0;

      *list = swap;
      (*list)->alloc_count += ALLOC_INC_COUNT;
    }

  memcpy((*list)->data + (*list)->count * (*list)->el_size, data, (*list)->el_size);
  (*list)->count++;
  return 1;
}

void block_list_remove(block_list_t *list, void *data)
{
  if (data == block_list_tail(list));
  
  else if (data == block_list_head(list) && block_list_count(list) > 1)
    memcpy(data, block_list_next(list, data), (block_list_count(list) - 1) * list->el_size);
  
  else
    {
      size_t i = 0;
  
      for (char *n = block_list_tail(list); n; n = block_list_prev(list, n), i++)
	if (n == data)
	  {
	    memcpy(n, block_list_next(list, n), i * list->el_size);
	    break;
	  }
    }

  list->count--;
}

void *block_list_next(block_list_t *list, void *data)
{
  if (data == block_list_tail(list))
    return NULL;

  return (char *) data + list->el_size;
}

void *block_list_prev(block_list_t *list, void *data)
{
  if (data == block_list_head(list))
    return NULL;

  return (char *) data - list->el_size;
}

void *block_list_itr_head(block_list_t *list, size_t n)
{
  if (!list->count)
    return NULL;

  else if (n > list->count - 1)
    return NULL;
  
  return (char *) block_list_head(list) + (n * list->el_size);
}

void *block_list_itr_tail(block_list_t *list, size_t n)
{
  if (n > list->count - 1)
    return NULL;
  
  return (char *) block_list_tail(list) - n * list->el_size;
}

void block_list_remove_tail(block_list_t *list)
{
  if (block_list_count(list))
    list->count--;
}

size_t block_list_count(block_list_t *list)
{
  return list->count;
}

void block_list_clear(block_list_t *list)
{
  list->count = 0;
}

void block_list_for_each(block_list_t *list, void (*callback)(void *, void *), void *userp)
{
  for (char *n = block_list_head(list); n; n = block_list_next(list, n))
    callback(n, userp);
}
