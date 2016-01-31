#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

#include "staticTreeBarrier.h"
#include "include/librace.h"

staticTreeBarrier *barrier;
const int N = 2, radix = 1;
int *A, *B, *C;
int var = 0, sum = 0;

void generate(void *arg)
{
	int *id = (int*) arg;
	store_32(A + *id, *id);
	store_32(B + *id, 2 * (*id));
	barrier->await(*id);
	
	store_32(C + *id, (*id + 1)*(load_32(&A[*id]) + load_32(&B[*id])) );
	printf("Stage1: My thread ID: %d\n===========\n", *id);
    printf("A[%d]:= %d\n", *id, load_32(A + *id));
    printf("B[%d]:= %d\n", *id, load_32(B + *id));

	barrier->await(*id);

	if (*id == 0)
	{
		for (int i=0; i<N; i++)
			sum += load_32(&C[i]);
	 
	}
	printf("Stage2: My thread ID: %d\n===========\n", *id);
    printf("C[%d]:= %d\n", *id, load_32(C + *id));
}

int user_main(int argc, char **argv)
{
	int i, j;
	barrier = new staticTreeBarrier(N, radix);
	int tid[N];

	thrd_t *thread_arr = (thrd_t *)malloc(N*sizeof(thrd_t));
	
	A = (int*)malloc(N*sizeof(int));
	B = (int*)malloc(N*sizeof(int));
	C = (int*)malloc(N*sizeof(int));
	for (i=0; i<N; i++) 
	{
		tid[i] = i;
		thrd_create(thread_arr+i, (thrd_start_t) generate, &tid[i]);
	}

	for (i=0; i<N; i++) 
		thrd_join(*(thread_arr+i));
	
	if(sum == 6) printf("\n**** Two stage Barrier is working just fine *****\n");
 	else printf("Something seems wrong! :-/ \n");
	
	return 0;
}
