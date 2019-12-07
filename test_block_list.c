/*
  $ cc test_block_list.c -L $PWD -l block_queue -Wl,-rpath,$PWD -o test_block_list && ./test_block_list 
*/

#include <stdio.h>
#include <time.h>
#include "block_list.h"

void print(void *n, void *context)
{
  printf("%d(%p) ", **(int **) n, (int **) n);
}

int main()
{
  block_list_t *list = block_list_new(5, sizeof(int));
  srand(time(NULL));
  
  for (unsigned i = 0; i < 10; i++)
    {
      int v = rand() % 100;
      
      if (!block_list_add(&list, &v))
	{
	  printf("Realloc failed\n");
	  block_list_del(list);
	  exit(0);
	}

      printf("%d: List count %d List Head %d List tail %d\n", v, block_list_count(list), *(int *) block_list_head(list), *(int *) block_list_tail(list));
    }

  printf("\nAccess\n======\n");  
  printf("Iterate from tail\n");
  for (int *n = block_list_tail(list); n; n = block_list_prev(list, n))
    printf("%d ", *(int *) n);

  printf("\nIterate from head\n");
  for (int i = 0; i < list->count; i++)
    printf("%d ", *(int *) block_list_itr_head(list, i));

  printf("\nRemove from tail\n");
  for (int *n = block_list_tail(list), i = 0; n && i < 3; n = block_list_prev(list, n), i++)
    block_list_remove(list, n);
  
  for (int i = 0; i < list->count; i++)
    printf("%d ", *(int *) block_list_itr_head(list, i));

  printf("\nRemove from head\n");
  for (int *n = block_list_head(list), i = 0; n && i < 2; n = block_list_next(list, n), i++)
    //if (i == 1)
      block_list_remove(list, n);
  
  for (int i = 0; i < list->count; i++)
    printf("%d ", *(int *) block_list_itr_head(list, i));

  printf("\n");
  block_list_del(list);

  printf("\nStoring Pointer\n===============\n");
  int V[] = { 4, 7, 8, 9 };
  block_list_t *list_p = block_list_new(5, sizeof(int *));

  for (unsigned i = 0; i < 4; i++)
    {
      int *p = &V[i];
      printf("%p ", *p);
      block_list_add(&list_p, &p);
    }

  block_list_for_each(list_p, print, NULL);
  printf("\n");

  for (int **i = block_list_head(list_p); i; i = block_list_next(list_p, i))
    printf("%d ", **(int **) i);
  
  printf("\n");
  block_list_del(list_p);
  return 0;
}
