#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mtwist.h"

static sem_t spaces;
static sem_t mutex;
static sem_t items;

typedef struct{
	int value;
	int wait;
}block;

block buffer[32];

unsigned int getRand();
//Functions to run as threads
void *producer(void *);
void *consumer(void *);
//List management
void push(int value, int timeout);
block pop();

int main(){
	//Seed the random number generator
	init_genrand(0);
	//initialize our sempahores
	if(	sem_init(&spaces,0,32)==-1 ||
		sem_init(&mutex,0,1)==-1 ||
		sem_init(&items,0,0)==-1 
	){
		printf("Error creating semaphores, exiting...");
		return 1;
	}
	pthread_t tProd;
	pthread_t tCons;
	//Start our two threads
	if(	pthread_create(&tProd,NULL,producer,NULL)!=0 ||
		pthread_create(&tCons,NULL,consumer,NULL)!=0
	){
		printf("Error creating threads, exiting...");
	}
	//Join our two threads.  This should not really happen because
	//each of our threads is an infinite loop.
	pthread_join(tProd,NULL);
	pthread_join(tCons,NULL);
	//Destroy our semaphores
	sem_destroy(&spaces);
	sem_destroy(&mutex);
	sem_destroy(&items);
	return 0;
}

unsigned int getRand(){
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
		//Using rdrand
		__asm__ __volatile__(
			"rdrand %0;"
			:"=r"(eax)
		);
		return eax;
	}
	else{	//Using mt19937
		return genrand_int31();	//defined in mtwist.c
	}
}

//Before calling, appropriate semaphores need to be waited on
//Specifically 'spaces' and 'mutex'
void push(int value, int timeout){
	int index;
	block newBlock;
	newBlock.value=value;
	newBlock.wait=timeout;
	sem_getvalue(&items,&index);
	//The "space" semaphore keeps track of the correct index.
	buffer[index-1]=newBlock;
}

//Before calling appropriate semaphores need to be waited on
//Specifically 'items' and 'mutex'
block pop(){
	int index;
	//Calling of this is controlled by semaphores so we access
	//the correct spot.
	sem_getvalue(&items,&index);
	return buffer[index-1];
}

void* producer(void *v){
	int value, timeout, rest;
	do{
		value=getRand();
		timeout=getRand()%8+2;
		rest=getRand()%5+3;
		//Wait for a time to edit things	
		sem_wait(&spaces);
		sem_wait(&mutex);
			push(value,timeout);
			printf("Produced Value: %d\n",value);
		sem_post(&mutex);
		sem_post(&items);
	}while(sleep(rest)==0);
	//Hit this if our sleep is interupted
	return NULL;
}

void* consumer(void *v){
	int value, timeout, rest;
	block item;
	do{
		sem_wait(&items);
		sem_wait(&mutex);
			//grab only what we need and release the lock.
			item=pop();
			printf("Consuming Value: %d\n",item.value);
		sem_post(&mutex);
		sem_post(&spaces);
		rest=item.wait;
	}while(sleep(rest)==0);
	//Hit this if the sleep is interupted.
	return NULL;
}

