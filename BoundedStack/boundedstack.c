#include <iostream>
#include <stdlib.h>
#include "librace.h"
#include "stdatomic.h"
#include <threads.h>

using namespace std;

struct spinlock {
        atomic_int lock;
};

#define UNLOCKED 0
#define LOCKED 1

struct spinlock * createLock() {
        struct spinlock *ptr = (struct spinlock *)malloc(sizeof(struct spinlock));
        atomic_init(&ptr->lock, UNLOCKED);
        return ptr;
}

void lock(struct spinlock *ptr) {
        while(1) {
                int tmp = atomic_load_explicit(&ptr->lock, memory_order_relaxed);
                if (tmp) {
                        thrd_yield();
                        continue;
                }
                int val = atomic_exchange_explicit(&ptr->lock, LOCKED, memory_order_acquire);
                if (val == UNLOCKED) break;
        }
}

void unlock(struct spinlock *ptr) {
        atomic_store_explicit(&ptr->lock, UNLOCKED, memory_order_release);
}

template <class T>
class Node {
public:
	T element;

	Node(T obj) {
		element = obj;
	}
	Node() {};
};

template <class T>
class Stack {
	int capacity;
	atomic_int top;
	Node<T> **bounded_array;
	struct spinlock *lock_top;

public:
	Stack (int bound) {
		int i;
		atomic_init(&top, -1);
		capacity = bound;
		lock_top = createLock();
		bounded_array = (Node<T> **)malloc(sizeof(Node<T> *)*capacity);
		for(i = 0; i < capacity; i++) { bounded_array[i] = new Node<T>(); }
	}

	void push (T);
	T pop();
};

template <class T>
void Stack<T>::push(T obj) {

	//is stack full?
	if(atomic_load_explicit(&top, memory_order_acquire) == capacity) {
		while(1) {
			if(atomic_load_explicit(&top, memory_order_acquire) < capacity) break;
			else thrd_yield();
		}
	}
	
	//acquire lock to push.
	lock(lock_top);

	Node<T> *new_one = new Node<T>(obj);
	bounded_array[atomic_load_explicit(&top, memory_order_acquire)] = new_one;
	atomic_fetch_add_explicit(&top, 1, memory_order_release);

	unlock(lock_top);
}

template <class T>
T Stack<T>::pop() {

	//is stack empty?
	if(atomic_load_explicit(&top, memory_order_acquire) == -1) {
		while(1) {
			if(atomic_load_explicit(&top, memory_order_acquire) >= 0) break;
			else thrd_yield();
		}
	}

	//acquire lock to pop.
	lock(lock_top);
	
	Node<T> *new_one = bounded_array[atomic_load_explicit(&top, memory_order_acquire)];
	atomic_fetch_sub_explicit(&top, 1, memory_order_release);

	unlock(lock_top);
	return new_one->element;
}
