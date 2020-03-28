/*
   $ cc test_bl.c -L $PWD -l blqueue -Wl,-rpath,$PWD -o test_bl && ./test_bl 
*/

#include <stdio.h>
#include <time.h>
#include "bl.h"

static void print(void *n, void *context)
{
  printf("%d(%p) ", **(int **) n, n);
}

int main()
{
  bl_t *list = bl_new(5, sizeof(int));
  srand(time(NULL));

  for (unsigned i = 0; i < 10; i++)
  {
    int v = rand() % 100;

    if (!bl_add(&list, &v))
    {
      printf("Realloc failed\n");
      bl_del(list);
      exit(0);
    }

    printf("%d: List count %zu List Head %d List tail %d\n", v, bl_count(list), *(int *) bl_head(list), *(int *) bl_tail(list));
  }

  printf("\nAccess\n======\n");  
  printf("Iterate from tail\n");
  for (int *n = bl_tail(list); n; n = bl_prev(list, n))
    printf("%d ", *(int *) n);

  printf("\nIterate from head\n");
  for (int i = 0; i < bl_count(list); i++)
    printf("%d ", *(int *) bl_itr_head(list, i));

  printf("\nRemove from tail\n");
  for (int *n = bl_tail(list), i = 0; n && i < 3; n = bl_prev(list, n), i++)
    bl_remove(list, n);

  for (int i = 0; i < bl_count(list); i++)
    printf("%d ", *(int *) bl_itr_head(list, i));

  printf("\nRemove from head\n");
  for (int *n = bl_head(list), i = 0; n && i < 2; n = bl_next(list, n), i++)
    //if (i == 1)
    bl_remove(list, n);

  for (int i = 0; i < bl_count(list); i++)
    printf("%d ", *(int *) bl_itr_head(list, i));

  printf("\n");
  bl_del(list);
  printf("\nStoring Pointer\n===============\n");
  int V[] = { 4, 7, 8, 9 };
  bl_t *list_p = bl_new(5, sizeof(int *));

  for (unsigned i = 0; i < 4; i++)
  {
    int *p = &V[i];
    printf("%p ", (void *) p);
    bl_add(&list_p, &p);
  }

  bl_for_each(list_p, print, NULL);
  putchar('\n');

  for (int **i = bl_head(list_p); i; i = bl_next(list_p, i))
    printf("%d ", **(int **) i);

  putchar('\n');
  bl_del(list_p);
  return 0;
}
