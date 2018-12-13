#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


void main(){
	pid_t spawnpid; 
	spawnpid = fork();
	switch(spawnpid){
		case -1:
			perror("child process failed to spawn!\n");
			exit(1);
		case 0:
			printf("Child Process: Parents PID: %d\n",getppid());
			break;
		default:
			printf("Parent Process: PID: %d\n", getpid());
	}
}
