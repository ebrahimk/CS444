/**************************************
 * Kamron Ebrahimi 
 * Group 10 
 * CS444
 * Concurrency 3 Part 1
 * References:
 * W. Stallings, Operating Systems Internals and Design Principles, 7th ed. Prentice Hall, 2012.  
 **************************************/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>

#define NUM 10

//Define Structure
typedef struct{
	sem_t mutex;
	sem_t block;
	int active;
	int waiting; 
	bool must_wait; 
}Shared_Resource;

typedef struct{
        pthread_t begin;
        int id;
}Consumer;

//Declare Globals
Shared_Resource pool; 

//Function Prototypes
void initPool();
void *execute(void *);
void use();


int main(){
        srand(time(NULL));
		
	//initialize the resource pool 
        initPool();

        //Initialize the threads 
        Consumer consumers[NUM];

        //Start our two threads
        int threadResult, i;
      	for(i = 0; i < NUM; i++){
		threadResult = pthread_create(&consumers[i].begin,NULL,execute, &consumers[i]);
        	assert(threadResult == 0);
		//set the id of the thread
		consumers[i].id = i;
	}

	for(i = 0; i < NUM; i++){
		pthread_join(consumers[i].begin,NULL);
	}	

	return 0;
}

/*********************************
 * Name:		initPool()
 * Preconditions:	Shared_Resource structure is declared
 * Postconditions:	Shared_Resource structures initialized
 * Description:		This function initializes all of the semaphore members and different 
			variables in a Shared_Resource structure.
 * Return:		N/A 
 * ******************************/
void initPool(){
        if(sem_init(&pool.mutex,0,1)==-1 || sem_init(&pool.block,0,0)==-1){
                printf("Semaphore creation failed...");
                exit(1);
        }
	pool.active = 0; 
	pool.waiting = 0;
	pool.must_wait = false; 	
}

/*********************************
 * Name:		use() 
 * Preconditions:	A thread has access to a shared resource
 * Postconditions:	The executing threads sleeps for a random amount of time
 * Description:		This function generates a random number then sleeps for the 
			duration of the generated number to simulate a thread performing some work
 * Return:		N/A
 * ******************************/
void use(Consumer* cons){
        int num = rand();
        num = abs(num%(7)+1);           //worked with 5 
        printf("Thread %d: Consuming resources %d seconds\n",cons->id,  num);
        sleep(num);
}

/*********************************
 * Name:		execute()
 * Preconditions:	A thread is initialized and set to execute this routine
 * Postconditions:	Threads execute concurrently allowing up to three threads to share a resource 
 *			and blocking all access to the resource until all three threads have left. 
 * Description:	 	With the use of global flag variables we have a means of indicating what state our system is in. 
 *			However we must onnly allow one thread to change the value of these flags at a time. 
 *			Thus the pool.mutex semaphore is locked before any section of code where pool.waiting 
 *			of pool.active is incremented or decremented. Using pool.block we can prevent threads from executing code 
 *			based off the value of our global flags. Lets follow the path of the first threads to execute this function.
 *			First we lock pool.mutex to allow exclusive access to the pool.waiting and pool.active flags.
 *			The pool.must_wait variable is false thus we execute the "else" statement. This indicates that the thread is gaining 
 *			access to the shared resources. The thread increments the number of active threads and we check to see if three 
 *			threads are active in which case pool.must_wait is set to "true". Finally we unlock out mutex so other threads
 *			can axecute that block of code. leaving the first conditional we no have access to the resources and use() is called.
 *			Other threads executing after the first will not take the first conditional branch until three threads have incremented pool.active 
 *			to the value of three at which point poo.must_wait is true. 
 *			If the fourth thread executes this function it will take the first conditional 
 *			increment the number of waiting threads exclusivly then attempt ot lock pool.block (initialized ot zero).
 *			This will cause this thread and all subsequent threads to block and increment the number of waiting threads. 
 *			The trick of cauing all waiting threads to block until all threads reliquish the resource granted that three threads gained access to the 
 *			resource is found in the second set of conditional statements. 
 *			The last thread leaving the shared resource will trigger the second conditional branch where the number of threads granted access to the 
 *			resource next is equal to the number of waiting threads up to the value three. The number of waiting threads pool.waiting is then decremented by this number
 *			and pool.active is set to this number. In order to allow only a number of threads equal to num_threads_next access to the resource 
 *			a while loop is entered in which the value of the block semaphore is incremented every iteration. This will unlock threads block within the 
 *			first conditional statement at sem_wait(&pool.block) to unlock and gain access to the resource. Finally is the num_threads_next 
 *			was equal to three, i.e. the number of threads just unlocked, the value of pool.must_wait is set to true causing the cycle to continue. *
 * Return: 		N/A
 * ******************************/
void* execute(void *cons){
	Consumer *my_cons = (Consumer*)cons;
	while(1){
		//mutex ensures mutual exclusion for global variables 
		sem_wait(&pool.mutex);
		if(pool.must_wait){
			pool.waiting = pool.waiting + 1; 
			printf("Thread %d must wait three threads are accessing the resource\n", my_cons->id);
			sem_post(&pool.mutex);
			sem_wait(&pool.block);
		}
		else{
			pool.active =pool.active +1;
			pool.must_wait = (pool.active == 3) ? true : false;
			sem_post(&pool.mutex); 
		}

		use(my_cons);
		printf("Thread %d releasing resources.\n", my_cons->id);

		sem_wait(&pool.mutex);
		pool.active = pool.active - 1;
		if(pool.active ==0){
			int num_threads_next;
			if(pool.waiting < 3)
				num_threads_next = pool.waiting;
			else
				num_threads_next = 3;
			pool.waiting = pool.waiting - num_threads_next; //reset the value of pool.waiting
			pool.active = num_threads_next;		 //Number of threads about to gain access to the resource 
			while(num_threads_next > 0){			 //iterates up to three times
				sem_post(&pool.block);	 //Unlock the block a number of times equal to the number of waiting threads up to three
				num_threads_next = num_threads_next -1;		 //This will allow thread block within the first conditional to execute
			}
			pool.must_wait = (pool.active == 3) ? true : false;	//Check if three threads were granted access to the resource
		}		
		sem_post(&pool.mutex);
	}
}


