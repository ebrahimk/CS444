#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/resource.h>

void main(){
	pid_t spawnpid, childpid;
	spawnpid = fork();
	switch(spawnpid){
		case -1:
			perror("child process failed to spawn!\n");
			exit(1);
		case 0:
			childpid = getpid(); 
			printf("Child Process: Nice Value: %d\n",getpriority(PRIO_PROCESS, 0));
			sleep(1);
			printf("Child Process: New Nice Value: %d\n",getpriority(PRIO_PROCESS, 0));
			break;
		default:
			sleep(1);
			printf("Changing the nice value of child process..\n");
			int result = setpriority(PRIO_PROCESS, spawnpid, 3);
			
	}
}
