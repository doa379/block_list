/*
  $ cc test_bltpool.c -L $PWD -l blqueue -Wl,-rpath,$PWD -o test_bltpool && ./test_bltpool
*/

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "bltpool.h"

void func_cb(arg_t ARG[])
{
  int *v = (int *) ARG[0].arg;
  printf("Job %d\n", *v);
  sleep(1);
  printf("Job %d complete\n", *v);
}

int main()
{
  bltpool_t *bltpool = bltpool_new();
  srand(time(NULL));

  for (unsigned i = 0; i < 10; i++)
    {
      int v = rand() % 100;
      bltpool_queue(bltpool, func_cb, 1, &v, sizeof v);
      printf("Data %d job added\n", v);
    }

  while (bltpool_job_count(bltpool))
    {
      printf("%zu jobs remaining\n", bltpool_job_count(bltpool));
      sleep(1);
    }

  bltpool_del(bltpool);
  return 0;
}
