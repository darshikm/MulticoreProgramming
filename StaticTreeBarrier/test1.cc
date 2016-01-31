#include <stdio.h>
#include <stdlib.h>
#include "staticTreeBarrier.h"


/*********** GLOBAL TEST CASE DEFINITIONS ***********/

int N_threads = 2, givenRadix = 1, sum = 0;
int *A, *B;
staticTreeBarrier *myBarrier;

void test(void *arg)
{
  int *id = (int *)arg, i;
  store_32(&A[*id], *id + 1);
  store_32(&B[*id], ((*id + 1)*2));

  myBarrier->await(*id);
  
  if(*id == 0)
  {
   //for(i = 0; i < N_threads; i++) 
   sum += load_32(&A[*id]) + load_32(&B[*id]) + 7;
   printf("The value of sum computed is: %d\n", load_32(&sum));
  }
  else
  {
   printf("My thread ID: %d\n===========\n", *id);
   printf("A[%d]:= %d\n", *id, load_32(A + *id));
   printf("B[%d]:= %d\n", *id, load_32(B + *id));
   store_32(&A[*id], *id + 15);
   store_32(&B[*id], ((*id + 15)*2));
  }
  
  myBarrier->await(*id);
  if(*id == 0)
  {
   for(i = 0; i < N_threads; i++) 
    sum += load_32(&A[i]) + load_32(&B[i]);
   printf("The value of sum computed is: %d\n", load_32(&sum));
  }


}

int user_main(int n, char **args) //Model checker requires user_main instead of main()
{
 int i = 0;
 myBarrier = new staticTreeBarrier(N_threads, givenRadix);
 
 int *tid = (int *)malloc(N_threads*sizeof(int));
 A = (int *)malloc(N_threads*sizeof(int));
 B = (int *)malloc(N_threads*sizeof(int));
 
 thrd_t pt0, ct1, ct2;
 
 for (i = 0; i < N_threads; i++)
 {
  tid[i] = i;
 }
 
 thrd_create(&pt0, (thrd_start_t) test, &tid[0]);
 thrd_create(&ct1, (thrd_start_t) test, &tid[1]);
 //thrd_create(&ct2, (thrd_start_t) test, &tid[2]);
 
 //thrd_join(ct2);
 thrd_join(ct1);
 thrd_join(pt0);

 if(sum == 61) printf("\n**** Single stage Barrier is working just fine *****\n");
 else printf("Something seems wrong! :-/ \n");
 return 0;
};
