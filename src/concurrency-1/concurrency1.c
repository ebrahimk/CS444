#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mt19937ar.h"
#include <assert.h>

<<<<<<< HEAD
//Define Item structure, this represents one item within the buffer 
=======
>>>>>>> b6b9eb246d8520fe1435c3c33019cc1dbc70b537
typedef struct{
	int value;
	int time;
}Item;

<<<<<<< HEAD
//Define synchronization structure
=======
>>>>>>> b6b9eb246d8520fe1435c3c33019cc1dbc70b537
typedef struct{
	Item container[32];
	int length;
	int next_in;
	int next_out;
	sem_t mutex;
	sem_t items;
	sem_t spaces;
}Queue;	

<<<<<<< HEAD
=======

unsigned int getRand();

>>>>>>> b6b9eb246d8520fe1435c3c33019cc1dbc70b537
//Functions to run as threads
void *produce(void *);
void *consume(void *);

//List management
<<<<<<< HEAD
unsigned int getRand();
=======
>>>>>>> b6b9eb246d8520fe1435c3c33019cc1dbc70b537
void push(Item item);
Item pop();
void initQueue();
void systemType(); 

//declare Globals
int SYSTEMTYPE;
Queue buffer; 

<<<<<<< HEAD
=======

>>>>>>> b6b9eb246d8520fe1435c3c33019cc1dbc70b537
int main(){
	//Seed the random number generator
	init_genrand(0);

	//determine the systemtype
	systemType(); 

	//initialize the Queue
	initQueue();

<<<<<<< HEAD
	//Initialize two pthread_t structures 
	pthread_t producer;
	pthread_t consumer;

=======
	pthread_t producer;
	pthread_t consumer;


>>>>>>> b6b9eb246d8520fe1435c3c33019cc1dbc70b537
	//Start our two threads
	int threadResult;
	threadResult = pthread_create(&producer,NULL,produce,NULL); 
	assert(threadResult == 0);
	threadResult = pthread_create(&consumer,NULL,consume,NULL);
	assert(threadResult == 0);

<<<<<<< HEAD
	//Join the two threads, this will cause the thread executing our main function, the parent thread to block until
	//the producer and consumer threads complete.  However both the producer and consumer threads are locked in infinitle loops.
	pthread_join(producer,NULL);
	pthread_join(consumer,NULL);
	
	//Destroy our semaphores. In theory this code will never be executed. 
=======

	//Join our two threads.  This should not really happen because
	//each of our threads is an infinite loop.
	pthread_join(producer,NULL);
	pthread_join(consumer,NULL);
	//Destroy our semaphores
>>>>>>> b6b9eb246d8520fe1435c3c33019cc1dbc70b537
	sem_destroy(&buffer.spaces);
	sem_destroy(&buffer.mutex);
	sem_destroy(&buffer.items);

	return 0;
}

<<<<<<< HEAD
/***********************************
 * Name:		initQueue() 
 * Preconditions:	Process is executed.
 * Postconditions:	The Queue synchronization structure is initialized. 
 * Description:		This function will create three semaphore structures for controlling thread access to the buffer 
 * 			as well as initialize the length of the structure and the Item array.
 * Return:		N/A 
 * *********************************/
=======
>>>>>>> b6b9eb246d8520fe1435c3c33019cc1dbc70b537
void initQueue(){
	if(     sem_init(&buffer.spaces,0,32)==-1 ||
			sem_init(&buffer.mutex,0,1)==-1 ||
			sem_init(&buffer.items,0,0)==-1
	  ){
		printf("Semaphore creation failed...");
		exit(1);
	}
	//set other parameters
	buffer.length = 32;
	buffer.next_in = 0;
	buffer.next_out = 0;
}

<<<<<<< HEAD
/***********************************
 * Name:		systemType()
 * Preconditions:	Process is executed.
 * Postconditions:	The process determines if the system on which it is running has a x86 architecture or not.
 * Description:		This function uses the "cpuid" assembly instruction to grab system credentials and places the 
 * 			informations within the "eax" register. This value is then compared to 0x40000000 to determine if the system 
 * 			has a x86 architecture or not. If so then the rdrand asm instruction can be used to generate random numbers.
 * 			Otherwise the Mersenne Twister is used. 
 * Return:		Sets the SYSTEMTYPE global variable. 
 * *********************************/
=======
>>>>>>> b6b9eb246d8520fe1435c3c33019cc1dbc70b537
void systemType(){
	//Setup our registers 
	unsigned int eax,ebx,ecx,edx;
	eax=0x01;	//cpuid uses this value
	__asm__ __volatile__(
			"cpuid;"
			:"=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
			: "a"(eax)
			);
	//Generate the number
	if(ecx & 0x40000000){	//Check the 30th bit
		SYSTEMTYPE = 1;
	}
	else{	//Using mt19937
		SYSTEMTYPE = -1;
	}
}

<<<<<<< HEAD
/***********************************
 * Name:		getRand() 
 * Preconditions:	The produced thread is executig the produce routine.
 * Postconditions:	A random number is returned via a method contingent on the machine architecture. 
 * Description:		This function checks the SYSTEMTYPE variable to determine if the system on which the process is
 * 			running is x86 architecture. If so the function calls the rdrand asm instruction and stores the result in 
 * 			the num variable c-expression. If the SYSTEMTYPe is -1 then the Mersenne Twister is used. The code for the twister
 * 			is contained in the mt19937ar header file. The min and max parameters allow the output to be constrained between a 
 * 			ceiling and floor value. 
 * Return:
 * *********************************/
=======
>>>>>>> b6b9eb246d8520fe1435c3c33019cc1dbc70b537
unsigned int getRand(int min, int max){
	int num;
	//Generate the number
	if(SYSTEMTYPE == 1){   //Check the 30th bit
		//Using rdrand
		__asm__ __volatile__(
				"rdrand %0;"
				:"=r"(num)
				);
	}
	else{   //Using mt19937
		num = genrand_int31(); //defined in mtwist.c
	}
<<<<<<< HEAD
	
	num = abs(num%(max-min)+1);
	return num; 
}

/***********************************
 * Name:		push() 
 * Preconditions:	The producer thread is executing the produce routine.
 * Postconditions:	This function will push an Item structure onto the container member of a Queue structure. 
 * Description:		This function first calls sem_wait on both the mutex and spaces semaphore. Sem_wait will decrement the value 
 * 			of the semaphore structure which is passed as a parameter. If the value of the semaphore after 
 * 			sem_wait is called is is greater than 0 the functino will return immediatley, if not the function will block until 
 * 			the value raises above zero. The spaces semaphore keeps track of the number of avaliable spaces within the container 
 * 			member of the Queue structure. Thus when an item is pushed onto the container we need to check to verify 
 * 			that the buffer has space for a new item. When spaces reaches zero there is no space left and the producer thread 
 * 			needs to block until the consumer can consume an item and increment the spaces semaphore. The mutex semaphore simple ensures 
 * 			that the consumer and producer threads do not try to access the buffer at the same time. 
 * 			The function then adds the item to the buffer and calls sem_post() on the mutex and items semaphores. Sem_post increments 
 * 			the argument semaphore. Thus the push routine increments the number of items in the buffer via the items semaphore and 
 * 			changes the mutex semaphore to 1, indicating that the mutex is unlocked. 
 * Return:		N/A
 * *********************************/
=======

	num = abs(num%(max-min));
	return num; 
}

>>>>>>> b6b9eb246d8520fe1435c3c33019cc1dbc70b537
void push(Item item) {
	sem_wait(&buffer.spaces);
	sem_wait(&buffer.mutex);

	buffer.container[buffer.next_in] = item;
	buffer.next_in = (buffer.next_in +1)%buffer.length;

	sem_post(&buffer.mutex);
	sem_post(&buffer.items);
}

<<<<<<< HEAD
/***********************************
 * Name:		pop()
 * Preconditions:	The consumer thread is executing the consume routine. 
 * Postconditions:	This function will pop an item off the container member of the Queue structure. 
 * Description:		This function is almost an exact mirror of the push() function explained above. The function first locks the mutex, and 
 * 			decrements the value of the items semaphore. If this value is 0, the function knows that the buffer is empty and 
 * 			no item is avaliable for consumption. The thread executing the function will block in this instance. 
 * 			If the value is not zero, we have avaliable items in the buffer, the function will pop and item and 
 * 			call sem_post on the mutex and spaces semaphore indicating that their is one less avaliable space in the 
 * 			buffer and that the mutex is unlocked. 
 * Return:		N/A
 * *********************************/
=======
>>>>>>> b6b9eb246d8520fe1435c3c33019cc1dbc70b537
Item pop() {
	sem_wait(&buffer.items);
	sem_wait(&buffer.mutex);

	Item item = buffer.container[buffer.next_out];
	buffer.next_out = (buffer.next_out +1)%buffer.length;

	sem_post(&buffer.mutex);
	sem_post(&buffer.spaces);

	return item;
}

<<<<<<< HEAD
/***********************************
 * Name:		produce()
 * Preconditions:	The producer thread is created.
 * Postconditions:	The producer routine is executed.
 * Description:		This function enters an infinite loop in which the producer thread waits 3 to 7 seconds then creates a new Item structure,
 * 			fills the value and consumption time of the Item structure and calls push() to place the item in the buffer. 
 * Return:		N/A
 * *********************************/
=======

>>>>>>> b6b9eb246d8520fe1435c3c33019cc1dbc70b537
void* produce(void *v){
	Item newItem;
	int wait;
	while(1){		
		wait = getRand(3, 7);
		sleep(wait);
		newItem.value = getRand(1, 100);
		newItem.time =getRand(2, 9);
		printf("Produced Item with value: %d\n", newItem.value);
		push(newItem);
	}	
}

<<<<<<< HEAD
/***********************************		
 * Name:		consume()
 * Preconditions:	The consumer thread was successfully created. 	
 * Postconditions:	The consumer thread executes the consume routine. 
 * Description:		This functions calls pop() grabbing a value of off the buffer, waits an amount of time equal to the consumption value 
 * 			then prints the value of the item which was just consumed.  
 * Return:		N/A
 * *********************************/
=======
>>>>>>> b6b9eb246d8520fe1435c3c33019cc1dbc70b537
void* consume(void *v){
	Item consumedItem;
	while(1){
		consumedItem = pop();
		sleep(consumedItem.time);
<<<<<<< HEAD
		printf("Consumed Item with value and this is the time to consume: %d\n", consumedItem.value);
=======
		printf("Consumed Item with value: %d\n", consumedItem.value);
>>>>>>> b6b9eb246d8520fe1435c3c33019cc1dbc70b537
	}
}
