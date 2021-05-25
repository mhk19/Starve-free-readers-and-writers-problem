<h1 align="center">Reading Assignment CSN-232</h1>
<h2 align="center"><b>Starve Free Readers-Writers Problem</b></h2>

The readers-writers problem deals with the synchronization of multiple processes trying to read or write upon a shared data. If two readers access the shared data simultaneously, there won't be any adverse effects. However, if a writer and some other process (either a reader or writer) access the database simultaneously, chaos may ensue.

The first and second readers-writers problem provide solution in which writer or reader can starve possibly. The third readers-writers problem provides a solution in which neither reader nor writer starve.

The third readers-writers problem requires that, a writer can write if there are no reader or writer. If there are readers or any writer, the writer has to wait and it enters a FIFO queue. After the readers already reading have completed their job, they signal for the writer to enter its critical section. Any reader or writer coming after this writer also goes in that queue. Therefore, after this writer has completed its job, the one which entered first in this queue, whether it is reader or writer, will enter its critical section next.

### Implementation of the FIFO queue.

```
// A node of the queue.
struct Node{
  Node* next;
  int p_id;
}

// The queue data structure
struct queue{
  	Node* Front, Rear;
	void push(int id){
		Node* n = new Node();
		n->p_id = id;
		if(Rear != NULL){
			Rear->next = n;
			Rear = n;
		}
		else{
			Front = Rear = n;
		}
	}
  
	int pop(){
		if(Front == NULL){
			return -1; // Error : underflow.
		}
		else{
			int id = Front->value;
			Front = Front->next;
			if(Front == NULL){
				Rear = NULL;
			}
			return id;
		}
	}
}
```

We will maintain this FIFO queue in the semaphore itself. Whenever a process executes wait operation and semaphore is negative, the process blocks itself and enters the queue.
The data structures used in the implementation of queue would be:
* `int value`
* `queue list`

### Implementation of the semaphore

```
// Implementation of the semaphore.
struct Semaphore{
    int value = 1;
    queue* Q = new queue();
    
    void wait(int p_id){
        value--;
        if(value < 0){
            Q->push(p_id);
            block(); //The process calling this function will block itself. This is 		   to prevent busy waiting.
        }
    }
    
    void signal(){
        value++;
        if(value <= 0){
            int pid = Q->pop();
            wakeup(pid); //this function will wakeup the process with the given pid.
        }
    }
}
```

The pseudo code for third readers-writers problem:

```
Semaphore* readers = new Semaphore();
Semaphore* mutex = new Semaphore();
Semaphore* writers = new Semaphore();
writer_sem->value = 0;
int num_started = 0;    	 // number of readers started reading.
int num_completed = 0;  	 // number of readers completed reading.
bool writer_waiting = false; // this indicated whether a writing is waiting.

void Reader(void *p_id){
	readers->wait(p_id); //wait on the readers semaphore.
	num_started++;
	readers->signal();

	//Read the data. This is the "critical section".

	mutex->wait(p_id); //wait on the mutex semaphore.
	num_completed++;
	if(writer_waiting && num_started == num_completed){
		writers->signal();
	}
	mutex->signal();
}

void Writer(void *p_id){
	readers->wait(p_id);
	mutex->wait(p_id);
	if(num_started == num_completed){
		mutex->signal();
	}
	else{
		writer_waiting = true;
		mutex->signal();
		writers->wait();
		writer_waiting = false;
	}

	//Write the data. This is the "critical section"

	readers->signal();
}
```

Explanation of the pseudo code-

* Multiple readers can read data simultaneously.
* When a writer is waiting it tells the readers that it is waiting by setting `writer_waiting` to true.
* When all the readers before the writer finish reading they signal the writer to access the data.
* When the writer is accessing the data, all the other processes which come after that go into queue of `readers` semaphore.
* After the writer finishes writing it signals the `readers` semaphore so that next process in the queue can access data.
* Also, to make the `writers` semaphore synchronizable across both the process where one process only executes `wait()` and other only executes `signal()`, we initialize it to `0`.
* This solution is starvation free for both readers and writers because there is no bias for anyone. Processes get the access to data in the order they have entered the queue. Readers will allow other readers in the queue to start reading with them but writers will block all the other processes waiting in the queue from executing until it is finished.

## References

* [arXiv:1309.4507](https://arxiv.org/ftp/arxiv/papers/1309/1309.4507.pdf)

* Abraham Silberschatz, Peter B. Galvin, Greg Gagne - Operating System Concepts