/*
  $ cc test_block_tpool.c -L $PWD -l block_queue -Wl,-rpath,$PWD -o test_block_tpool && ./test_block_tpool
*/

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "block_tpool.h"

void func_cb(void *context, void *data)
{
  int *v = (int *) data;
  printf("Job %d\n", *v);
  sleep(1);
  printf("Job %d complete\n", *v);
}

int main()
{
  block_tpool_t *block_tpool = block_tpool_new(5, NULL);
  srand(time(NULL));

  for (unsigned i = 0; i < 5; i++)
    {
      int v = rand() % 100;
      block_tpool_queue(block_tpool, func_cb, &v, sizeof v);
      printf("Data %d job added\n", v);
    }

  while (block_list_count(block_tpool->list_jobs_q))
    {
      printf("%d jobs remaining\n", block_list_count(block_tpool->list_jobs_q));
      sleep(1);
    }

  block_tpool_del(block_tpool);
  return 0;
}
