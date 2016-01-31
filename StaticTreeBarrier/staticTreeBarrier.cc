#include <stdio.h>
#include <stdlib.h>
#include "staticTreeBarrier.h"


/*********** GLOBAL TEST CASE DEFINITIONS ***********/

int N_threads = 3, givenRadix = 1, var = 0;
staticTreeBarrier *myBarrier;

void threadA(void *arg)
{
  int *id = (int *)arg;
  store_32(&var, 1);
  myBarrier->await(*id);
}

void threadB(void *arg)
{
  int *id = (int *)arg;
  myBarrier->await(*id);
  printf("var = %d\n", load_32(&var));
}

int user_main(int n, char **args)
{
 int i = 0;
 myBarrier = new staticTreeBarrier(N_threads, givenRadix);
 
 int *tid = (int *)malloc(N_threads*sizeof(int));
 thrd_t pt0, ct1, ct2;
 
 for (i = 0; i < N_threads; i++)
 {
  tid[i] = i;
 }
 
 thrd_create(&pt0, (thrd_start_t) threadA, &tid[0]);
 thrd_create(&ct1, (thrd_start_t) threadB, &tid[1]);
 thrd_create(&ct2, (thrd_start_t) threadB, &tid[2]);
 
 thrd_join(ct2);
 thrd_join(ct1);
 thrd_join(pt0);

 return 0;
};
