#include <string.h>
#include "bltpool.h"

static void job_del(void *node, void *userp)
{
  bltpool_job_t *job = (bltpool_job_t *) node;

  for (unsigned i = 0; i < job->nargs; i++)
    if (job->ARG[i].size)
      free(job->ARG[i].arg);

  bl_t *Q = (bl_t *) userp;
  bl_remove(Q, node);
}

void bltpool_clear(bltpool_t *bltpool)
{
  pthread_mutex_lock(&bltpool->mutex);
  bl_for_each(bltpool->Q, job_del, bltpool->Q);
  pthread_mutex_unlock(&bltpool->mutex);
}

size_t bltpool_job_count(bltpool_t *bltpool)
{
  pthread_mutex_lock(&bltpool->mutex);
  size_t count = bl_count(bltpool->Q);
  pthread_mutex_unlock(&bltpool->mutex);
  return count;
}

static void queue_pop(bltpool_t *bltpool)
{
  void *head = bl_head(bltpool->Q);
  job_del(head, bltpool->Q);
}

bool bltpool_queue(bltpool_t *bltpool, void (*func)(arg_t []), unsigned n, ...)
{
  bltpool_job_t job = { .func = func, .nargs = n };
  va_list valist;
  va_start(valist, n);

  for (unsigned i = 0; i < n; i++)
  {
    arg_t arg = va_arg(valist, arg_t);

    if (arg.size)
    {
      void *data;
      
      if ((data = malloc(arg.size)) == NULL)
      {
        for (unsigned j = 0; j < i - 1; j++)
          if (job.ARG[j].size)
            free(job.ARG[j].arg);

        return 0;
      }

      memcpy(data, arg.arg, arg.size);
      arg.arg = data;
    }
    
    job.ARG[i] = arg;
  }
  
  va_end(valist);
  pthread_mutex_lock(&bltpool->mutex);
  
  if (bl_add(&bltpool->Q, &job))
  {
    pthread_mutex_unlock(&bltpool->mutex);
    pthread_cond_signal(&bltpool->cond_var);
  }

  else
  {
    for (unsigned i = 0; i < n; i++)
      if (job.ARG[i].size)
        free(job.ARG[i].arg);

    pthread_mutex_unlock(&bltpool->mutex);
    return 0;
  }

  return 1;
}

static void *worker_th(void *userp)
{
  bltpool_t *bltpool = userp;

  while (1)
  {
    pthread_mutex_lock(&bltpool->mutex);

    while (!bl_count(bltpool->Q) && !bltpool->quit)
      pthread_cond_wait(&bltpool->cond_var, &bltpool->mutex);

    if (bltpool->quit)
    {
      pthread_mutex_unlock(&bltpool->mutex);
      return NULL;
    }

    bltpool_job_t *job = bl_head(bltpool->Q);
    pthread_mutex_unlock(&bltpool->mutex);
    (job->func)(job->ARG);
    pthread_mutex_lock(&bltpool->mutex);
    queue_pop(bltpool);
    pthread_mutex_unlock(&bltpool->mutex);
  }

  return NULL;
}

void bltpool_del(bltpool_t *bltpool)
{
  bltpool->quit = 1;
  bltpool_clear(bltpool);
  pthread_cond_signal(&bltpool->cond_var);
  pthread_join(bltpool->pth, NULL);
  pthread_cond_destroy(&bltpool->cond_var);
  pthread_mutex_destroy(&bltpool->mutex);
  bl_del(bltpool->Q);
  free(bltpool);
  bltpool = NULL;
}

bltpool_t *bltpool_new(void)
{
  bltpool_t *bltpool = calloc(1, sizeof *bltpool);

  if (bltpool == NULL)
    return NULL;

  bltpool->Q = bl_new(sizeof(bltpool_job_t));

  if (bltpool->Q == NULL)
  {
    free(bltpool);
    return NULL;
  }

  pthread_mutex_init(&bltpool->mutex, NULL);
  pthread_cond_init(&bltpool->cond_var, NULL);
  pthread_create(&bltpool->pth, NULL, worker_th, (void *) bltpool);
  return bltpool;
}
