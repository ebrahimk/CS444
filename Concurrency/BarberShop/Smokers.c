/**************************************
 * Kamron Ebrahimi 
 * Group 10 
 * CS444
 * Concurrency 4 Smokers Solution  
 * References: Little Book of Semaphores, Downey, Allen B. 
 **************************************/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>

#define NUM 3 

//Define Structures
typedef struct{
        pthread_t begin;
        int id;
}Smoker;

typedef struct{
        pthread_t begin;
        int id;
}Agent;

typedef struct{
        pthread_t begin;
        int id;
}Pusher;

//Define Globals
sem_t agentSem; 
sem_t tobacco; 
sem_t paper; 
sem_t match;

sem_t mutex;

sem_t tobaccoSem; 
sem_t paperSem;
sem_t matchSem; 

bool isTobacco, isPaper, isMatch;

//Function Prototypes
void initStructs(); 
void* agent_one(void*);
void* agent_two(void*);
void* agent_three(void*);

void* smoker_one(void*);
void* smoker_two(void*);
void* smoker_three(void*);

void* pusherA(void*); 
void* pusherB(void*); 
void* pusherC(void*); 

int main(){
	srand(time(NULL));

	//initialize synchronization structures 
	initStructs(); 	

	//Initialize the threads 
	Smoker smokers[NUM];  
	Pusher pushers[NUM];
	Agent agents[NUM];

	//Start our two threads
	int threadResult;

	//Start the smoker threads first
	threadResult = pthread_create(&smokers[0].begin,NULL,smoker_one, &smokers[0]);
	assert(threadResult == 0);
	smokers[0].id = 0;

	threadResult = pthread_create(&smokers[1].begin,NULL,smoker_two, &smokers[1]);
	assert(threadResult == 0);
	smokers[1].id = 1;

	threadResult = pthread_create(&smokers[2].begin,NULL,smoker_three, &smokers[2]);
	assert(threadResult == 0);
	smokers[2].id = 2;

	//Start the agent hreads 
	threadResult = pthread_create(&agents[0].begin,NULL,agent_one, &agents[0]);
	assert(threadResult == 0);
	agents[0].id = 0;

	threadResult = pthread_create(&agents[1].begin,NULL,agent_two, &agents[1]);
	assert(threadResult == 0);
	agents[1].id = 1;

	threadResult = pthread_create(&agents[2].begin,NULL,agent_three, &agents[2]);
	assert(threadResult == 0);
	agents[2].id = 2;

	//Start the pusher threads
	threadResult = pthread_create(&pushers[0].begin,NULL,pusherA, &pushers[0]);
	assert(threadResult == 0);
	pushers[0].id = 0;

	threadResult = pthread_create(&pushers[1].begin,NULL,pusherB, &pushers[1]);
	assert(threadResult == 0);
	pushers[1].id = 1;

	threadResult = pthread_create(&pushers[2].begin,NULL,pusherC, &pushers[2]);
	assert(threadResult == 0);
	pushers[2].id = 2;

	int i;
	for(i = 0; i < NUM; i++){
		pthread_join(smokers[i].begin,NULL);
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
	if(sem_init(&agentSem,0,1)==-1 || sem_init(&tobacco,0,0)==-1 ||sem_init(&paper,0,0)==-1 || sem_init(&match,0,0)==-1|| sem_init(&tobaccoSem,0,0) == -1 || sem_init(&paperSem,0,0) == -1 || sem_init(&matchSem,0,0)==-1|| sem_init(&mutex,0,1)==-1){
		printf("Semaphore creation failed...");
		exit(1);
	}
	isTobacco = false;
	isMatch = false;
	isPaper = false; 	 
}

/*********************************
 * Name:		makeCig()	
 * Preconditions:	The two resources obtained by the given smakoer are produced 
 * Postconditions:	The smoker makes a cigarette
 * Description:		This fuunction generates a random number and the calling thread waits for the given amount of time.
 * Return:		N/A
 * ******************************/
void makeCig(Smoker* smoker,char* component){
	int num = rand();
	num = abs(num%(9)+1);
	printf("SMOKER %d RECIEVED %s AND IS MAKING CIGARETTE\n",smoker->id, component);
	sleep(2);
}

/*********************************
 * Name:		smokeCig()
 * Preconditions:	Thread recieves both of the resources required to roll a cigarette 
 * Postconditions:	The thread "smokes" 
 * Description:		This function causes the thread to simply wait after recieving the required resources 		
 * Return:		N/A
 * ******************************/
void smokeCig(Smoker* smoker){
	int num = rand();
        num = abs(num%(9)+1);	
	printf("SMOKER %d SMOKING CIGARETTE\n",smoker->id);
	sleep(num);
	printf("SMOKER %d DONE SMOKING\n", smoker->id);
}

/*********************************
 * Name:		smoker_NUM() 
 * Preconditions:	smoker threads are properly initialized
 * Postconditions:	The function executes the proper smoker thread routine
 * Description:		This function enters an infinite loop in which the thread locks the tobaccoSem semaphore. This semaphore is used by the
 *			pusher thread to signal the smoker corresponding smoker thread with tobacco that the corresponding ingredients are avaliable.
 *			Thus if the ingrediaents are not avaliable, i.e. "tobaccoSem" has a value of zero and the smoekr thread calls wait
 *			the thread will block until the resources avaliable. The smoker thread then rolls a cigareete via a call to makeCig(), indicates
 *			that is recieved the resources from the pusher thread by signalling agentSem then proceeds to smoke the cigarette via a call to 
 *			smokeCig(). Each smoker thread follows this exact same pattern though each thread has different resources. 
 * Return:		N/A 
 * ******************************/
void* smoker_one(void *cons){
	Smoker *my_smoke = (Smoker*)cons;
	while(1){
		sem_wait(&tobaccoSem); 
		makeCig(my_smoke,"MATCHES AND PAPER");
		sem_post(&agentSem);
		smokeCig(my_smoke);
	}
}

void* smoker_two(void *cons){
        Smoker *my_smoke = (Smoker*)cons;
        while(1){
                sem_wait(&matchSem);
                makeCig(my_smoke,"PAPER AND TOBACCO");
                sem_post(&agentSem);
                smokeCig(my_smoke);
        }
}

void* smoker_three(void *cons){
        Smoker *my_smoke = (Smoker*)cons;
        while(1){
                sem_wait(&paperSem);
                makeCig(my_smoke,"TOBACCO AND MATCHES");
                sem_post(&agentSem);
                smokeCig(my_smoke);
        }
}

/*********************************
 * Name:		agent_NUM
 * Preconditions:	Agent threads are initialized.
 * Postconditions:	Agent protocol is executed by three agent threads. 
 * Description:		Agent threads simply provide two resources, thus the agent thread first locks the agentSem semaphore 
 *			this indicates that resources are being provided and facilitates communication between smoker and agent threads. 
 *			The agent thread then signals to resource semaphores to indicate that the resources are presented to the smokers and are 
 * 			currently avaliable.
 * Return:		N/A 
 * ******************************/
void* agent_one(void *cons){
        Agent *my_agent = (Agent*)cons;
        while(1){
		sem_wait(&agentSem);
		sem_post(&tobacco);
		sem_post(&paper);		
	}	
}

void* agent_two(void *cons){
        Agent *my_agent = (Agent*)cons;
        while(1){
                sem_wait(&agentSem);
                sem_post(&paper);
                sem_post(&match);
        }
}

void* agent_three(void *cons){
        Agent *my_agent = (Agent*)cons;
        while(1){
                sem_wait(&agentSem);
                sem_post(&tobacco);
                sem_post(&match);
        }
}

/*********************************
 * Name:		pusher_NUM()
 * Preconditions:	pusher threads initialized properly.		
 * Postconditions:	communication between the agent and smoker threads is completed so as to prevent deadlock. 
 * Description:		The pusher threads first lock one of the three substance semaphores signalled by the agent thread indicating that the avaliable 
 *			resource has been noticed and is being pushed to the smoker threads. The function then locks a mutex to allow exclusive access to
 *			the boolean variables indicating what is avaliable. The thread then checks to see what resource were made avaliable by previous threads.
 *			If the agent produces a match and the corresponding pusher thread then locks the match semaphore if enters the conditional statement 
 *			in an attempt to determine what other resopurce is avaliable. Thus if the match pusher observes that isTobacco is true it knows that 
 *			pusher A already executed and will signal paperSem indicating to the smoker with paper that matches and tobacco are avaliable. The 
 *			thread will then reassign is Tobacco to false. In this fashion any pusher that runs and finds that non of the other booleans 
 *			are true will hit the last else statement and just indicate that the given resource is now avaliable though it will not unlock
 *			any smoker thread. 
 * Return:		N/A
 * ******************************/
void* pusherA(void *cons){
	Pusher *my_push = (Pusher*)cons;
	while(1){
		sem_wait(&tobacco); 
		sem_wait(&mutex); 
		if(isPaper){
			isPaper = false; 
			sem_post(&matchSem);
		}
		else if(isMatch){
			isMatch = false; 
			sem_post(&paperSem);
		}
		else
			isTobacco = true; 
		sem_post(&mutex);
	}
}

void* pusherB(void *cons){
	Pusher *my_push = (Pusher*)cons;
	while(1){
		sem_wait(&match);
		sem_wait(&mutex);
		if(isTobacco){
			isTobacco = false;
			sem_post(&paperSem);
		}
		else if(isPaper){
			isPaper = false;
			sem_post(&tobaccoSem);
		}
		else
			isMatch = true;
		sem_post(&mutex);
	}
}

void* pusherC(void *cons){
	Pusher *my_push = (Pusher*)cons;
	while(1){
		sem_wait(&paper);
		sem_wait(&mutex);
		if(isMatch){
			isMatch = false;
			sem_post(&tobaccoSem);
		}
		else if(isTobacco){
			isTobacco = false;
			sem_post(&matchSem);
		}
		else
			isPaper = true;
		sem_post(&mutex);
	}
}






