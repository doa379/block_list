#ifndef BLOCK_TPOOL_H
#define BLOCK_TPOOL_H

#include <pthread.h>
#include "bl.h"

typedef struct
{
  void (*func)(void *);
  void *arg;
  size_t arg_size;
} bltpool_job_t;

typedef struct
{
  bl_t *Q;
  pthread_mutex_t mutex;
  pthread_cond_t cond_var;
  pthread_t pth;
  bool quit;
} bltpool_t;

bltpool_t *bltpool_new(void);
void bltpool_del(bltpool_t *);
void bltpool_clear(bltpool_t *);
size_t bltpool_job_count(bltpool_t *);
bool bltpool_queue(bltpool_t *, void (*)(void *), void *, size_t);

#endif
