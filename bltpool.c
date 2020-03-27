#include <string.h>
#include "bltpool.h"

static void job_del(void *node, void *userp)
{
  bltpool_job_t *job = (bltpool_job_t *) node;

  if (job->arg_size)
    free(job->arg);

  bl_t *list = (bl_t *) userp;
  bl_remove(list, node);
}

void bltpool_clear(bltpool_t *bltpool)
{
  pthread_mutex_lock(&bltpool->mutex);
  bl_for_each(bltpool->list_jobs_q, job_del, bltpool->list_jobs_q);
  pthread_mutex_unlock(&bltpool->mutex);
}

size_t bltpool_job_count(bltpool_t *bltpool)
{
  size_t count;
  pthread_mutex_lock(&bltpool->mutex);
  count = bl_count(bltpool->list_jobs_q);
  pthread_mutex_unlock(&bltpool->mutex);
  return count;
}

static void queue_pop(bltpool_t *bltpool)
{
  void *head = bl_head(bltpool->list_jobs_q);
  job_del(head, bltpool->list_jobs_q);
}

void bltpool_queue(bltpool_t *bltpool, void (*func)(void *), void *arg, size_t size)
{
  bltpool_job_t job = {
    .func = func,
    .arg = arg,
    .arg_size = size
  };

  if (size)
  {
    job.arg = malloc(size);
    memcpy(job.arg, arg, size);
  }

  pthread_mutex_lock(&bltpool->mutex);
  bl_add(&bltpool->list_jobs_q, &job);
  pthread_mutex_unlock(&bltpool->mutex);
  pthread_cond_signal(&bltpool->cond_var);
}

static void *worker_th(void *userp)
{
  bltpool_t *bltpool = userp;

  while (1)
  {
    pthread_mutex_lock(&bltpool->mutex);

    while (!bl_count(bltpool->list_jobs_q) && !bltpool->quit)
      pthread_cond_wait(&bltpool->cond_var, &bltpool->mutex);

    if (bltpool->quit)
    {
      pthread_mutex_unlock(&bltpool->mutex);
      return NULL;
    }

    bltpool_job_t *job = bl_head(bltpool->list_jobs_q);
    pthread_mutex_unlock(&bltpool->mutex);
    void (*func)(void *) = job->func;
    func(job->arg);
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
  bl_del(bltpool->list_jobs_q);
  free(bltpool);
  bltpool = NULL;
}

bltpool_t *bltpool_new(size_t init_alloc_size)
{
  bltpool_t *bltpool = malloc(sizeof *bltpool);
  bltpool->quit = 0;
  bltpool->list_jobs_q = bl_new(init_alloc_size, sizeof(bltpool_job_t));
  pthread_mutex_init(&bltpool->mutex, NULL);
  pthread_cond_init(&bltpool->cond_var, NULL);
  pthread_create(&bltpool->pth, NULL, worker_th, (void *) bltpool);
  return bltpool;
}
