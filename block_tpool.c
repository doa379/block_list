#include <string.h>
#include "block_tpool.h"

static void job_del(void *node, void *userp)
{
  block_job_t *job = (block_job_t *) node;

  if (job->arg_size)
    free(job->arg);

  block_list_t *list = (block_list_t *) userp;
  block_list_remove(list, node);
}

void block_tpool_clear(block_tpool_t *block_tpool)
{
  pthread_mutex_lock(&block_tpool->mutex);
  block_list_for_each(block_tpool->list_jobs_q, job_del, block_tpool->list_jobs_q);
  pthread_mutex_unlock(&block_tpool->mutex);
}

static void queue_pop(block_tpool_t *block_tpool)
{
  void *head = block_list_head(block_tpool->list_jobs_q);
  job_del(head, block_tpool->list_jobs_q);
}

void block_tpool_queue(block_tpool_t *block_tpool, void (*func)(void *), void *arg, size_t size)
{
  block_job_t job = {
    .func = func,
    .arg = arg,
    .arg_size = size
  };

  if (size)
    {
      job.arg = malloc(size);
      memcpy(job.arg, arg, size);
    }
  
  pthread_mutex_lock(&block_tpool->mutex);
  block_list_add(&block_tpool->list_jobs_q, &job);
  pthread_mutex_unlock(&block_tpool->mutex);
  pthread_cond_signal(&block_tpool->cond_var);
}

static void *worker_th(void *userp)
{
  block_tpool_t *block_tpool = userp;
  
  while (1)
    {
      pthread_mutex_lock(&block_tpool->mutex);

      while (!block_list_count(block_tpool->list_jobs_q) && !block_tpool->quit)
	pthread_cond_wait(&block_tpool->cond_var, &block_tpool->mutex);

      if (block_tpool->quit)
	{
	  pthread_mutex_unlock(&block_tpool->mutex);
	  return NULL;
	}
      
      block_job_t *job = block_list_head(block_tpool->list_jobs_q);
      pthread_mutex_unlock(&block_tpool->mutex);
      void (*func)(void *) = job->func;
      func(job->arg);
      pthread_mutex_lock(&block_tpool->mutex);
      queue_pop(block_tpool);
      pthread_mutex_unlock(&block_tpool->mutex);
    }

  return NULL;
}

void block_tpool_del(block_tpool_t *block_tpool)
{
  block_tpool->quit = 1;
  block_tpool_clear(block_tpool);
  pthread_cond_signal(&block_tpool->cond_var);
  pthread_join(block_tpool->pth, NULL);
  pthread_cond_destroy(&block_tpool->cond_var);
  pthread_mutex_destroy(&block_tpool->mutex);
  block_list_del(block_tpool->list_jobs_q);
  free(block_tpool);
  block_tpool = NULL;
}

block_tpool_t *block_tpool_new(size_t init_alloc_size)
{
  block_tpool_t *block_tpool = malloc(sizeof *block_tpool);
  block_tpool->quit = 0;
  block_tpool->list_jobs_q = block_list_new(init_alloc_size, sizeof(block_job_t));
  pthread_mutex_init(&block_tpool->mutex, NULL);
  pthread_cond_init(&block_tpool->cond_var, NULL);
  pthread_create(&block_tpool->pth, NULL, worker_th, (void *) block_tpool);
  return block_tpool;
}
