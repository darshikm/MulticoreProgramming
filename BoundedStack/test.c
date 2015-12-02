#include <iostream>
#include <stdlib.h>
#include "librace.h"
#include "stdatomic.h"
#include <threads.h>

using namespace std;

Stack<int> *myStack;

void test(void *obj) {
	int *id = (int *)obj, data;

	if(*id == 0) {
		cout<<"Request of pop by thread1\n";
		data = myStack->pop();
		cout<<"The popped value obtained := "<<data<<endl;
	}

	if(*id == 1) {
		cout<<"Request of push by thread2\n";
		data = 4;
		myStack->push(data);
		cout<<"The value pushed := "<<data<<endl;
	}

	if(*id == 2) {
		cout<<"Request of push by thread3\n";
		data = 7;
		myStack->push(data);
		cout<<"The value pushed := "<<data<<endl;
	}
}

int user_main(int argc, char **argv) {
	int tid[3], i;
	thrd_t thread1, thread2, thread3;
	myStack = new Stack<int>(2);

	for (i = 0; i < 3; i++) {
		tid[i] = i;
 	}
	
	thrd_create(&thread1, (thrd_start_t) test, &tid[0]);
    thrd_create(&thread2, (thrd_start_t) test, &tid[1]);
    thrd_create(&thread3, (thrd_start_t) test, &tid[2]);

    thrd_join(thread1);
    thrd_join(thread2);
    thrd_join(thread3);

	return 0;
}
