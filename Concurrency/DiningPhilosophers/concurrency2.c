#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define NUM 5
#define THINKING 2
#define HUNGRY 1
#define EATING 0

//Create structs
struct Philosopher{
	pthread_t execute;
	char* name;
	int state;
	int num;
};

struct Fork{
	sem_t fork;
	int num;
};

//Declare Globals
struct Fork silverware [NUM];
struct Philosopher guests[NUM];
sem_t mutex;

//Left and right functions for getting the respective forks adjacent to each philosopher
void beginTheMeal();
void setTable();
int left(int i);
int right(int i);
void think(struct Philosopher* phil);
void eat(struct Philosopher* phil);
void get_forks(struct Philosopher* phil);
void put_forks(struct Philosopher* phil);
void test();
int left_person(int i);

//Each philosophers will engage in the dining activity: Locke, Socrates, Voltaire, Confucius, Sartre
void *dine(void *);

int main(){
	srand(time(NULL));

	//initialize mutex
	sem_init(&mutex, 0, 1);

	//create forks semaphores and "set the table" i.e. initialize the semaphores
	setTable();

	//Initialize philosophers and begin dining
	beginTheMeal();

	//Join the two threads, this will cause the thread executing our main function, the parent thread to block until
	//the dining process completes
	int i;
	for(i = 0; i < NUM; i++){
		pthread_join(guests[i].execute,NULL);
	}

	return 0;
}

/***********************************
 * Name:		left() 
 * Preconditions:	Threads are successfully created as well as all semaphores.
 * Postconditions:	The left index is returned
 * Description:		This function returns the index of the left philosopher to the index located at index i. 
 * Return:		int
 * *********************************/
int left(int i){
	return (i+5)% NUM ;
}

/***********************************
 * Name:		right() 
 * Preconditions:	Threads are successfully created as well as semaphores.
 * Postconditions:	The right index is returned.	
 * Description:		This function returns the index of the right philosopher to the index located at i. 
 * Return:		int
 * *********************************/
int right(int i){
	return (i + 9)%NUM;
}

/***********************************
 * Name:		get_forks() 
 * Preconditions:	Threads have been properly initialized.
 * Postconditions:	The philosopher which is passed as an argument attempts to grab a fork.
 * Description:		This function first locks the mutex so threads will block if they reach this function. The function sets the Philosopher 
 * 			object to the HUNGRY state and calls the test() function. After exiting the test() funciton the mutex is unlocked so the 
 * 			next thread can call the test() function. Finally the function calls sem_wait() on the fork to the left of the philosopher.
 * 			This signals that the fork can be grabbed by other philosophers. Finally the thread goes to sleep. This prevents thread 
 * 			starvation.		
 * Return:		N/A
 * *********************************/
void get_forks(struct Philosopher* phil){
	
	sem_wait(&mutex);
	phil->state = HUNGRY;	
	printf("%s is hungry\n", phil->name);;
	
	test(phil);

	sem_post(&mutex);

	sem_wait(&silverware[phil->num].fork);
	sleep(1);
}

/***********************************
 * Name:		put_forks()
 * Preconditions:	Threads have been properly initialized.
 * Postconditions:	The thread will reliquish shared resources.
 * Description:		This function first locks the mutex semaphore and changes the executing threads status to THINKING.
 * 			The function then calls think() causing the philosopher to think for a time being. Finally to 
 * 			facilitate concurrency the function calls test() on the the two philosophers adjacent to the philosopher executing the thread.
 *			Thus if the philosopher which executed this function was consuming shared resources and preventing their neighbors from eating 
 *			the test() funciton will ensure that these neighbors have immediate access to the forks so they can eat.
 * Return:		N/A 
 * *********************************/
void put_forks(struct Philosopher* phil){
	sem_wait(&mutex);
	phil->state = THINKING;
	printf("%s put down forks %d and %d\n", phil->name, silverware[right(phil->num)].num, silverware[left(phil->num)].num);
	printf("%s is now thinking\n", phil->name);
	think(phil);
	//printf("Me: %s , left phil: %s, right phil %s\n",phil->name, guests[left_person(phil->num)].name, guests[right(phil->num)].name); 
	test(&guests[left_person(phil->num)]);
	test(&guests[right(phil->num)]);
	sem_post(&mutex);
}

/***********************************
 * Name:		test()
 * Preconditions:	get_forks() or put_forks is called. 
 * Postconditions:	A philosopher will eat. 
 * Description:		This function checks the state of the calling philosopher to see if they are HUNGRY and also verifies that the philosophers neighbors
 * 			are both in the EATING state. If the aforementioned canditions are satisfied then the function changes the status of the calling philosopher 
 * 			to EATING and and calls the eat() function in which the philosopher maintains a locked mutex and sleeps for a short time. After exiting 
 * 			the eat function the function signals the fork to there left indicating that the resource is avaliable.
 * Return:		N/A
 * *********************************/

void test(struct Philosopher* phil){
	if(phil->state == HUNGRY && guests[left_person(phil->num)].state != EATING && guests[right(phil->num)].state != EATING){
		phil->state = EATING;
		printf("%s picked up forks %d and %d\n", phil->name, silverware[right(phil->num)].num, silverware[left(phil->num)].num);
		printf("%s is now eating\n", phil->name);
		eat(phil);
		sem_post(&silverware[phil->num].fork);
	}
}

/***********************************
 * Name:		left_person()
 * Preconditions:	grabs the philosopher to the left of the philosopher at index i.
 * Postconditions:	returns the number corresponding to the returned philosopher.
 * Description:		This function was required because I kept running into an indexing issue. Accessing the left philosopher is 
 * 			more complicated than the left fork to a philosopher. Thus to grab the index of the left neighbor this funciton was created.
 * Return:		int
 * *********************************/
int left_person(int i){
	return (i+6)%NUM;
}

/***********************************
 * Name:		beginTheMeal()
 * Preconditions:	An array of Philosophers is successfully initialized.
 * Postconditions:	The philosophers are given names and their respective threads are executed. 
 * Description:		This function provides names to each of the philosophers, gives them an indexing number initializes them to the THINKING state. 
 * 			Finally the function initializess the pthreads and passes the philosopher object as a function argument to the thread execution funtion.
 * Return:		N/A
 * *********************************/
void beginTheMeal(){
	int threadResult, i;
	guests[0].name = "Nietzsche";
	guests[1].name = "Locke";
	guests[2].name = "Leibniz";
	guests[3].name = "Confucius";
	guests[4].name = "Sartre";

	//set a philosophers to have a unique id and to be in the THINKING state 
	for(i = 0; i < NUM; i++){
		guests[i].num = i; 
		guests[i].state = THINKING; 
		threadResult = pthread_create(&guests[i].execute,NULL,dine,(void*) &guests[i]);
		assert(threadResult == 0);
	}
}

/***********************************
 * Name:		setTable()
 * Preconditions:	N/A 
 * Postconditions:	semaphore array of forks are initialized
 * Description:		This function loops through the array of Fork objects, gives them an index number and initializes the semaphore object.
 * Return:		N/A
 * *********************************/
void setTable(){
	int i;
	for(i = 0; i < NUM; i++){
		silverware[i].num = i;	
		if(sem_init(&silverware[i].fork,0,0)==-1){
			printf("Semaphore creation failed...");
			exit(1);
		}
	}
}

/***********************************
 * Name:		think() 	
 * Preconditions:	put_forks() is called
 * Postconditions:	The philosopher waits for some random amount of time
 * Description:		This function generates a random number between 1 and 20 and sleeps for that amount of time.
 * Return:		N/A
 * *********************************/
void think(struct Philosopher* phil){
	int num = rand();
	num = abs(num%(19)+1);
	//printf("This is num think: %d\n", num);
	sleep(num);
}

/***********************************
 * Name:		eat() 
 * Preconditions:	test() is called
 * Postconditions:	The philosopher eats for a random amount of time.
 * Description:		This function generates a random number between 2 and 7 seconds and sleeps for that amount of time.
 * Return:		N/A
 * *********************************/
void eat(struct Philosopher* phil){
	int num = rand();
	num = abs(num%(7)+2);
//	        printf("This is eat: %d\n", num);
	sleep(num);
}

/***********************************
 * Name:		dine() 
 * Preconditions:	The execution threads are initialized.
 * Postconditions:	The philosophers begin their meal and execute concurrently.
 * Description:		This function enters an infinite loop in which philosophers wait(1) to prevent starvation then call get_forks() and put_forks().
 * Return:		N/A
 * *********************************/
//verify that I am grabbing the proper philosopher struct
void* dine(void* phil){
	struct Philosopher *my_phil = (struct Philosopher*)phil;
	//srintf("This is phil: %s\n",(struct Phlosopher*) my_phil->name);
	while(1){
	//	think(my_phil);
		sleep(1);
		get_forks(my_phil);
	//	eat(my_phil);
		sleep(0);
		put_forks(my_phil);	
	}
}
