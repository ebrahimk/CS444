/**************************************
 * Kamron Ebrahimi 
 * Group 10 
 * CS444
 * Concurrency 4 Barbers solution
 * References: Little Book of Semaphores, Downey, Allen B. 
 **************************************/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>

#define NUM 10 
#define CHAIRS 4 
//Define Structures
typedef struct{
        pthread_t begin;
        int id;
}Customer;

//Define Globals
int duration;
int customers_in_shop; 
sem_t mutex; 
sem_t customer; 
sem_t barber; 
sem_t customerDone;
sem_t barberDone;
pthread_t barbershop; 

//Function Prototypes
void initStructs(); 
void get_hair_cut(); 
void balk();
void get_hair_cut();
void cut_hair(); 
void* execute(void*);
void* barberShop(void*);

int main(){
	srand(time(NULL));

	//initialize synchronization structures 
	initStructs(); 	

	//Initialize the threads 
	Customer customers[NUM];  

	//Start our two threads
	int threadResult, i;

	//start the barber thread first
	threadResult = pthread_create(&barbershop,NULL,barberShop,NULL); 

	//Start the customer threads
	for(i = 0; i < NUM; i++){
		threadResult = pthread_create(&customers[i].begin,NULL,execute, &customers[i]);
		assert(threadResult == 0);
		//set the id of the thread
		customers[i].id = i;
	}

	for(i = 0; i < NUM; i++){
		pthread_join(customers[i].begin,NULL);
	}	

	return 0;
}

/*********************************
 * Name:		initStructs() 
 * Preconditions:	semaphore objects are declared 
 * Postconditions:	semaphore objects are initialized
 * Description:		This function initializes all of the synchronization structures used to implement the barber shop solution.
 * Return:		N/A
 * ******************************/
void initStructs(){
	if(sem_init(&mutex,0,1)==-1 || sem_init(&customer,0,0)==-1 ||sem_init(&barber,0,0)==-1 || sem_init(&customerDone,0,0)==-1|| sem_init(&barberDone,0,0) == -1){
		printf("Semaphore creation failed...");
		exit(1);
	}
	customers_in_shop = 0; 
}

/*********************************
 * Name:		cut_hair()
 * Preconditions:	Barber has an active customer thread in shared portion of code. 
 * Postconditions:	Barber performs a "hair cut".
 * Description:		Barber thread generates a random number and waits for that given amount of time.
 * Return:		N/A 
 * ******************************/
void cut_hair(){
	int num = rand();
	duration = abs(num%(6)+1);
	printf("BARBER CUTTING HAIR FOR %d MINUTES\n", duration);
	sleep(duration);
}

/*********************************
 * Name:		get_hair_cut()
 * Preconditions:	Customer thread enters shared code and has exclusive access to the barber thread. 
 * Postconditions:	Customer gets a "hair cut"
 * Description:		This function simply prints information to the console.
 * Return:		N/A
 * ******************************/
void get_hair_cut(Customer* cons){
	printf("CUSTOMER %d GETTING HAIRCUT\n",cons->id);
}

/*********************************
 * Name:		barberShop() 
 * Preconditions:	Barber thread initialized properly
 * Postconditions:	The barber thread synchronzes with customer thread to provide exclusive access "haircuts"
 * Description:		This function will first call wait on the customer semaphore, the customer semaphore keeps track of the number of customers
 *                      within the barber shop, thus if their are no customers in the shop, i.e. the "customer" semaphore has a value of zero
 *                      then the barber thread will simply block until one arrives. The thread then increments the barber semaphore to indicate that the
 *                      the barber is avaliable to provide a haircut to any blocked customer threads. The thread then waits for a random
 *                      duration of time performing the haircut. Finally the thread calls sem_wait on the customerDone semaphore. This semaphore along
 *                      with barberDone ensure that the both the barber thread providing the haircut and the customer thread recieving the haircut wait
 *                      for the exact same amount of time.
 * Return:		N/A
 * ******************************/
void* barberShop(void* v){
	while(1){
		sem_wait(&customer); 
		sem_post(&barber);

		//CUT HAIR
		cut_hair(); 

		sem_wait(&customerDone);
		sem_post(&barberDone); 	
	}
}



/*********************************
 * Name:		execute() 	
 * Preconditions:	customer threads are initialized properly
 * Postconditions:	customers synchronize with the barber thread to provide exclusive access to the barbers "haircut" services.
 * Description:		This function first sleeps to help mitigate thread starvation. The function utilizes the mutex semaphore to grant exclusive access to 
 * 			the "customer_in_shop" variable. The customer thread first locks the mutex then checks to see if the shop is full. If the shop is full
 * 			customer thread turns around and leaves, i.e. conditionally branches to the TRYAGAIN label. If the shop is not full then the thread will 
 * 			"enter the shop" and increment the counter "customers_in_shop" then unlock the mutex. The thread then signals the customer semaphore 
 * 			to indicate to the barber thread that a customer is in the shop and wake the barber thread if is is "sleeping" (blocked). The customer 
 * 			thread then calls semwait on the barber thread to gain exclusive access to the critical section fo the code. If the barber is not busy 
 * 			then the barber semaphore locks and the customer thread gains exclusive access to the resource, i.e. gets a haircut. 
 * 			After getting a haircut the thread will unlock the customerDone semaphore and the call sem_wai() ofn the barberDone semaphore to synchronize
 * 			the barber thread and the customer thread which just recieved a "haircut." Finally the customer thread will lock the mutex again in order 
 * 			to decrement the "customers_in_shop" variable and indicate that the customer is leaving the shop. 
 * Return:		N/A
 * ******************************/
void* execute(void *cons){
	Customer *my_cust = (Customer*)cons;
	while(1){
		
		TRYAGAIN: sleep(2);
		sem_wait(&mutex); 
		if(customers_in_shop == CHAIRS){
			sem_post(&mutex);
			goto TRYAGAIN;
		}

		customers_in_shop = customers_in_shop + 1; 
		printf("Customer %d entered the shop, %d customers in the shop\n", my_cust->id, customers_in_shop);

		sem_post(&mutex); 

		sem_post(&customer);
		sem_wait(&barber);

		//GET HAIR CUT 			
		get_hair_cut(my_cust);

		sem_post(&customerDone); 
		sem_wait(&barberDone);

		sem_wait(&mutex);
		customers_in_shop = customers_in_shop - 1;
		printf("Customer %d leaving the shop, %d customers in the shop\n", my_cust->id, customers_in_shop); 
		sem_post(&mutex);
	}
}


