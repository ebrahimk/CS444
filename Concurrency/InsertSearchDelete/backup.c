#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>

#define NUM_SEARCH 5
#define NUM_DELETE 5
#define NUM_INSERT 5

//Define Structures
typedef struct{
	pthread_t exe_search; 
	int id;
}Searcher;

typedef struct{
        pthread_t exe_delete;
        int id;
}Deleter;

typedef struct{
        pthread_t exe_insert;
        int id;
}Inserter;  
 
struct Node{
	int value; 
	struct Node* next;
};

typedef struct{
	struct Node* head;
	int size; 
}List_Head; 

//Declare Global Variables
List_Head list_head;
int insertCount;
int searchCount; 
 
sem_t mutex;
sem_t mutex_in;
sem_t insertMutex;		//Indicate that only one inserter is present
sem_t noSearcher;		//Indicate that no searhcer or Inserter is present
sem_t noInserter;		//USED FOR DELETERS Must hold both of those to enter 

void initVariables();
void *search(void *);
void *delete(void *);
void *insert(void *);
void insert_node(Inserter* my_inserter);
void search_list(Searcher* my_searcher);
void delete_node(Deleter* my_deleter);


int main(){
        srand(time(NULL));
		
	//initialize the list head 
        initVariables();

        //Initialize the threads 
        Inserter inserters[NUM_INSERT];
	Deleter deleters[NUM_DELETE];
	Searcher searchers[NUM_SEARCH];

        //Start our threads
        int threadResult, i;
      	for(i = 0; i < NUM_INSERT; i++){
		threadResult = pthread_create(&inserters[i].exe_insert,NULL, insert, &inserters[i]);
        	assert(threadResult == 0);
		inserters[i].id = i;
	}
        for(i = 0; i < NUM_DELETE; i++){
                threadResult = pthread_create(&deleters[i].exe_delete,NULL, delete, &deleters[i]);
                assert(threadResult == 0);
                deleters[i].id = i;
        }
        for(i = 0; i < NUM_SEARCH; i++){
                threadResult = pthread_create(&searchers[i].exe_search,NULL, search, &searchers[i]);
                assert(threadResult == 0);
                searchers[i].id = i;
        }

	//Ensure that main threads continues execution
	for(i = 0; i < NUM_INSERT; i++){
		pthread_join(inserters[i].exe_insert,NULL);
	}	

	return 0;
}

void initVariables(){
        if(sem_init(&insertMutex,0,1)==-1 || sem_init(&noSearcher,0,1)==-1 
	|| sem_init(&noInserter,0,1) == -1 || sem_init(&mutex, 0, 1) == -1 || sem_init(&mutex_in, 0, 1)==-1){          
	      	printf("Semaphore creation failed...");
                exit(1);
        }
	insertCount = 0;
	searchCount = 0;
	list_head.size = 0; 
	list_head.head = NULL; 
}


void insert_node(Inserter* my_inserter){
	int data = rand();
	data = abs(data%(10)+1); 			//data will range from one to ten
	//This is how we determine that we are at the end of the list

	//If we have no list we need to generate one. 
	if(list_head.head == NULL){
		list_head.head = (struct Node*)malloc(sizeof(struct Node)); 
		list_head.head->next =NULL;
		list_head.head->value = data;
		printf("INSERTER %d adding the %d node with value %d\n", my_inserter->id, list_head.size, data);
	}
	else{
		//we iterate through the list and append the new node to the tail incrementing the size of the list
		struct Node* current;
		struct Node* new_node;
		current = list_head.head;			//set the iterator to the head of the list

		while(current->next != NULL){
			current = current->next;	 
		}
		new_node = (struct Node*)malloc(sizeof(struct Node));
		new_node->value = data;
		new_node->next = NULL;                          //the tail node must always be pointing to null for its next attribute

		//add the new node increment the size of the list 
		current->next = new_node; 	
		list_head.size = list_head.size + 1; 
		printf("INSERTER %d adding the %d node with value %d\n", my_inserter->id, list_head.size, new_node->value);
	}
}


void search_list(Searcher* my_searcher){
	//generate a random number within the range of the number of nodes in the list
	if(list_head.size != 0){
		int node_num = rand() % list_head.size, i; 
		struct Node* current;
		current = list_head.head;
		for(i =0; i<node_num; i++){
			current = current->next; 
		}
		printf("SEARCHER %d reading the value of node %d. VALUE: %d\n",my_searcher->id, node_num, current->value);
			
	}
}

void delete_node(Deleter* my_deleter){
	if(list_head.size != 0){
		list_head.size = list_head.size - 1;
		struct Node* current; 
		struct Node* temp; 
		current = list_head.head; 
		int i, node_num = rand() % list_head.size+1;
		for(i = 0; i < node_num; i++){
			current = current->next;
		}
		//current is now the node to be deleted 
		temp = current->next;
		current->value = current->next->value;
		current->next = temp->next;
		free(temp);
		 

		//list_head.size = list_head.size -1;
		printf("DELETER: deleting  node number %d. List is now %d in length\n", node_num, list_head.size);
		
	}
}


void* search(void *search_thread){
	Searcher *my_searcher = (Searcher*)search_thread;
	while(1){
		sleep(2);
		sem_wait(&mutex);
		searchCount = searchCount + 1; 
		if(searchCount == 1){
			sem_wait(&noSearcher); 
		}
		sem_post(&mutex);
		
		search_list(my_searcher); 

		sem_wait(&mutex);
		searchCount = searchCount - 1; 
		if(searchCount == 0){
			sem_post(&noSearcher);	
		}
		sem_post(&mutex);					
	}
}



void* insert(void *insert_thread){
	Inserter *my_inserter = (Inserter*)insert_thread;
	while(1){
		sleep(2);

		sem_wait(&mutex_in);
		insertCount = insertCount + 1; 
		if(insertCount == 1){
			sem_wait(&noInserter);

		}
		sem_post(&mutex_in);
		sem_wait(&insertMutex); 

		insert_node(my_inserter); 

		sem_post(&insertMutex);
		sem_wait(&mutex_in);
		insertCount = insertCount - 1;
		
		if(insertCount == 0)
			sem_post(&noInserter);
		sem_post(&mutex_in);		        
	}
}

void* delete(void *delete_thread){
	Deleter *my_deleter = (Deleter*)delete_thread;
	while(1){
		sleep(5);

		sem_wait(&noSearcher);		
		sem_wait(&noInserter);

		delete_node(my_deleter);

		sem_post(&noInserter);
		sem_post(&noSearcher);
	}
}
