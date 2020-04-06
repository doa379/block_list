#ifndef BLOCK_TPOOL_H
#define BLOCK_TPOOL_H

#include <pthread.h>
#include <stdarg.h>
#include "bl.h"

#define MAX_ARGS 4

typedef struct
{
  void *arg;
  size_t size;
} arg_t;

typedef struct
{
  void (*func)(arg_t []);
  arg_t ARG[MAX_ARGS];
  unsigned nargs;
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
bool bltpool_queue(bltpool_t *, void (*)(arg_t []), unsigned, ...);

#endif
