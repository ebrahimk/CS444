#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mt19937ar.h"
#include <assert.h>

typedef struct{
	int value;
	int time;
}Item;

typedef struct{
	Item container[32];
	int length;
	int next_in;
	int next_out;
	sem_t mutex;
	sem_t items;
	sem_t spaces;
}Queue;	


unsigned int getRand();

//Functions to run as threads
void *produce(void *);
void *consume(void *);

//List management
void push(Item item);
Item pop();
void initQueue();
void systemType(); 

//declare Globals
int SYSTEMTYPE;
Queue buffer; 


int main(){
	//Seed the random number generator
	init_genrand(0);

	//determine the systemtype
	systemType(); 

	//initialize the Queue
	initQueue();

	pthread_t producer;
	pthread_t consumer;


	//Start our two threads
	int threadResult;
	threadResult = pthread_create(&producer,NULL,produce,NULL); 
	assert(threadResult == 0);
	threadResult = pthread_create(&consumer,NULL,consume,NULL);
	assert(threadResult == 0);


	//Join our two threads.  This should not really happen because
	//each of our threads is an infinite loop.
	pthread_join(producer,NULL);
	pthread_join(consumer,NULL);
	//Destroy our semaphores
	sem_destroy(&buffer.spaces);
	sem_destroy(&buffer.mutex);
	sem_destroy(&buffer.items);

	return 0;
}

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

	num = abs(num%(max-min));
	return num; 
}

void push(Item item) {
	sem_wait(&buffer.spaces);
	sem_wait(&buffer.mutex);

	buffer.container[buffer.next_in] = item;
	buffer.next_in = (buffer.next_in +1)%buffer.length;

	sem_post(&buffer.mutex);
	sem_post(&buffer.items);
}

Item pop() {
	sem_wait(&buffer.items);
	sem_wait(&buffer.mutex);

	Item item = buffer.container[buffer.next_out];
	buffer.next_out = (buffer.next_out +1)%buffer.length;

	sem_post(&buffer.mutex);
	sem_post(&buffer.spaces);

	return item;
}


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

void* consume(void *v){
	Item consumedItem;
	while(1){
		consumedItem = pop();
		sleep(consumedItem.time);
		printf("Consumed Item with value: %d\n", consumedItem.value);
	}
}
