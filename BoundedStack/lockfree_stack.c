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
	atomic<int> top;
	Node<T> **bounded_array;
	
public:
	Stack (int bound) {
		int i;
		top = -1;
		capacity = bound;
		bounded_array = (Node<T> **)malloc(sizeof(Node<T> *)*capacity);
		for(i = 0; i < capacity; i++) { bounded_array[i] = new Node<T>(); }
	}

	void push (T);
	T pop();
};

template <class T>
void Stack<T>::push(T obj) {
	Node<T> *new_one = new Node<T>(obj);
	int current_top = top.load(memory_order_acquire);
	bool status = false;

	while(1) {
		if(current_top < capacity) {
			status = top.compare_exchange_strong(current_top, current_top + 1);
			if(status) {
				bounded_array[current_top] = new_one;
			}
			/*else: retry */
		} 
		else thrd_yield();
	}
}

template <class T>
T Stack<T>::pop() {
	int current_top; 
	bool status = false;

	while(1) {
		current_top = top.load(memory_order_acquire);
		if(current_top == -1) {
			while(1) {
				if((current_top = top.load()) >= 0) break;
				else thrd_yield();
			}
		}

		status = top.compare_exchange_strong(current_top, current_top - 1);
		if(status) {
			return bounded_array[current_top]->element;
		}
		/* else retry */
	}
}
