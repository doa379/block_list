#ifndef BLOCK_TPOOL_H
#define BLOCK_TPOOL_H

#include <pthread.h>
#include "block_list.h"

typedef struct
{
  void (*func)(void *);
  void *arg;
  size_t arg_size;
} block_job_t;

typedef struct
{
  block_list_t *list_jobs_q;
  pthread_mutex_t mutex;
  pthread_cond_t cond_var;
  pthread_t pth;
  bool quit;
} block_tpool_t;

block_tpool_t *block_tpool_new(size_t);
void block_tpool_del(block_tpool_t *);
void block_tpool_clear(block_tpool_t *);
void block_tpool_queue(block_tpool_t *, void (*)(void *), void *, size_t);

#endif
