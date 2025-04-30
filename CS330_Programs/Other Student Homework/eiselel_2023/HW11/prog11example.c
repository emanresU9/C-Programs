// message queue and semaphore example - for CS330 mq_send, mq_receive, sem
// Written by Scott Spetka 4/11/2020 
// used example: 
// https://stackoverflow.com/questions/3056307/how-do-i-use-mqueue-in-a-c-program-on-a-linux-based-system
// For semaphores, used exaple:
// https://www.softprayog.in/programming/posix-semaphores
// Pipe example code used:
// https://www.geeksforgeeks.org/c-program-for-pipe-in-linux/

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// NOTE: SEMAPHORES AND MESSAGE QUEUES ARE SYSTEM OBJECTS
//       YOU MAY HAVE TO DEFINE YOUR OWN INSTEAD OF USING
//       /scott
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


#include <fcntl.h>  // Pipe and Semaphore
#include <stdlib.h> 
#include <sys/wait.h> 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>        /* For mode constants */
#include <sys/types.h>
#include <errno.h>
#include <mqueue.h>
#include <unistd.h> // fork

  

// Semaphore
#include <semaphore.h>

#define SEM_MUTEX_NAME "/newsnewem-mutex"

#define cprocs 2
#define QUEUE_NAME "/scott"
#define MAX_SIZE 10
#define MSG_STOP    "exit"

#define CHECK(x) \
    do { \
        if (!(x)) { \
            fprintf(stderr, "%s:%d: ", __func__, __LINE__); \
            perror(#x); \
            exit(-1); \
        } \
    } while (0) \

void doRecv();
void doSend();

// array of 2 file descriptors 
// a[0] is for reading  
// and a[1] is for writing over a pipe  
int a[2];  
char buff[10]; 

int main(){

    sem_t *mutex_sem;
    if ((mutex_sem = sem_open (SEM_MUTEX_NAME, O_CREAT, 0660, 0)) == SEM_FAILED) {
	perror ("sem_open server"); exit (1);
    }

  
    // opening of pipe using pipe(a)    
    if (pipe(a) == -1)  
    { 
        perror("pipe"); // error in pipe 
        exit(1); // exit from the program 
    } 
  
  
    for (int i=0; i<cprocs; i++){
    // child process because return value zero 
    if (fork() == 0) {
        printf("Hello from Child!\n"); 
	doRecv();
    }
    // parent process because return value non-zero. 
    else{
        printf("Parent! started child %d\n",i+1); 
    }
    } 
    // Parent
    doSend(mutex_sem);

}
// Child processes execute this code to receive from master
void doRecv(){
    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_SIZE + 1];
    int must_stop = 0;

    sem_t *mutex_sem;


#ifdef MSGSES
    /* initialize the queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    /* create the message queue */
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    CHECK((mqd_t)-1 != mq);
#endif // MSGSES

    if ((mutex_sem = sem_open (SEM_MUTEX_NAME, O_RDONLY)) == SEM_FAILED) {
	perror ("sem_open client"); exit (1);
    }

    do {
        ssize_t bytes_read;

        printf("Wait: process %d \n", getpid());
        // P (mutex_sem);
        if (sem_wait (mutex_sem) == -1) {
	    perror ("sem_wait: mutex_sem"); exit (1);
        }

  
    	// reading pipe now buff is equal to "code" 
        printf("Received: process %d ", getpid() );
        *buff='\1';
	while (*buff!='\0'){
        	read(a[0], buff, 1); ;
		printf("%c",*buff);
	}
	printf("\n");

#ifdef MSGSES
        /* receive the message */
        bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);
        CHECK(bytes_read >= 0);

        buffer[bytes_read] = '\0';
        if (! strncmp(buffer, MSG_STOP, strlen(MSG_STOP)))
        {
            must_stop = 1;
        }
        else
        {
            printf("Received: process %d %s\n", getpid(), buffer);
        }
#endif // MSGSES
    } while (!must_stop);
}

void doSend(sem_t *mutex_sem) {
    mqd_t mq;
    char buffer[MAX_SIZE];

    //sem_t *mutex_sem;

#ifdef MSGSES
    /* open the mail queue */
    mq = mq_open(QUEUE_NAME, O_WRONLY);
    CHECK((mqd_t)-1 != mq);
#endif // MSGSES

    // sem_t *sem_open(const char *name, int oflag,
                       // mode_t mode, unsigned int value);
    //sem_t *mysem = sem_open(const char *name, O_CREAT,
                        //O_CREAT, cprocs);



    printf("Send to server (enter \"exit\" to stop it):\n");

    do {
        printf("> ");
        fflush(stdout);

        memset(buffer, 0, MAX_SIZE);
        fgets(buffer, MAX_SIZE, stdin);

    // writing a string "code" in pipe 
    //write(a[1], "code", 5);  

    // Note, the length of the strlen does not include the '\0' NULL
    write(a[1], buffer, strlen(buffer)+1);  
    //printf("\n"); 
	
#ifdef MSGSES
        /* send the message */
        CHECK(0 <= mq_send(mq, buffer, MAX_SIZE, 0));
#endif // MSGSES
        printf("Sent: process %d %s\n", getpid(), buffer);

        // Release mutex sem: V (mutex_sem)
        if (sem_post (mutex_sem) == -1) {
	    perror ("sem_post: mutex_sem"); exit (1);
        }

    } while (strncmp(buffer, MSG_STOP, strlen(MSG_STOP)));

#ifdef MSGSES
    /* cleanup */
    CHECK((mqd_t)-1 != mq_close(mq));
#endif // MSGSES

    if (sem_unlink (SEM_MUTEX_NAME) == -1) {
        perror ("sem_unlink"); exit (1);
    }

}
