/**
*   Author: Tomas Bruckner
*   Date: 4.4.2015    
*   Description: 
*       Example of two processes sharing a variable.
*       make && ./example_advanced
*
*       If you are getting SIGSEGV, try:
*           rm /dev/shm/sem.example_advanced
**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <time.h>
#include <signal.h>

void set_resources();
void do_stuff();
void free_resources();

sem_t *semaphore;
int *sharedCounter = NULL, *notSharedCounter = NULL, sharedCounterId = 0;

int main(){
    setbuf(stdout, NULL);  // for valid printing

    set_resources();

    pid_t processPid = fork();

    if(processPid == 0){
	srandom( getpid() );

        for(int i = 0; i < 5; i++)
        {
            do_stuff("Child");

            usleep( random() % 500 * 1000 );  
        }

        exit(0);
    }
    else if(processPid != -1){  
	srandom( getpid() );

        for(int i = 0; i < 5; i++)
        {
            do_stuff("Parent");
            
            usleep( random() % 500 * 1000 );  
        }
    } 
    else{
        // handle error
    }

    waitpid(processPid, NULL, 0);

    free_resources();

    return 0;
}

void set_resources(){
    if( (sharedCounterId = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1 ){ 
        // handle error
    }
    
    if( (sharedCounter = (int *) shmat(sharedCounterId, NULL, 0)) == NULL ){ 
        // handle error
    }
 
    if( (notSharedCounter = (int *) malloc(sizeof(int))) == NULL ){
        // handle error
    }

    *sharedCounter = 1;
    *notSharedCounter = 1;
    
    if( (semaphore = sem_open("/example_advanced", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED ){ 
        // handle error
    }
}

void do_stuff(char * processName){
    sem_wait(semaphore); // lock critical section
    printf( "%s| not shared: %d shared: %d\n", processName, (*notSharedCounter)++, (*sharedCounter)++ );
    sem_post(semaphore); 
}

void free_resources(){
    free(notSharedCounter);
    shmctl(sharedCounterId, IPC_RMID, NULL);
    sem_close(semaphore);
    sem_unlink("/example_advanced");
}

