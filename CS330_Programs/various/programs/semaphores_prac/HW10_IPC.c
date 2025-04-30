#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <mqueue.h>
#include <stdbool.h>
#include <time.h>
#include <sys/wait.h>

/*
Syntax: ./a.out <filename>
This program reads the file then sporatically
sends the contents of each line to a queue. 
Worker processes are spawned and they receive
the lines from the queue but must be holding a 
sem.  
*/

#define QUEUE_NAME "/work_log"
#define MSG_SIZE 20
#define CHECK(cond, msg) \
    do {\
        if (cond) {\
            fprintf(stderr, "Error: %s (in file: %s at line %d)\n", msg, __FILE__, __LINE__);\
            exit(EXIT_FAILURE);\
        }\
    } while (0)

void worker(mqd_t mq, sem_t *lock, sem_t *count, int id) {
    char buff[MSG_SIZE];
    char msg[MSG_SIZE];
    ssize_t byte_size;
    int sleep_durration;
    int ret;
    bool done = false;

    while(!done) {
        //Try to enter critical region
        ret = sem_wait(lock);
        CHECK((ret == -1), "sem_wait");

        // Attempt to wait on the count semaphore without blocking
        while (sem_trywait(count) == -1) {
            sem_post(lock);
            usleep(1000);
        }

        //Pull from the queue, store in buff
        byte_size = mq_receive(mq, buff, sizeof(buff), NULL);
        CHECK((byte_size == -1), "mq_receive");
        buff[byte_size] = '\0';
        strcpy(msg, buff); //buff -> msg
        printf("Worker %d received msg: %s\n", id, msg);

        //If msg is equal to "EXIT" then set done to true and release the sem
        if (strcmp(msg, "EXIT") == 0){
            done = true;
            printf("Worker %d received the EXIT msg\n", id);
            sem_post(lock);
        }
        else {
            //Pull from the queue the tasks execution time
            byte_size = mq_receive(mq, buff, sizeof(buff), NULL);
            CHECK(byte_size == -1, "mq_receive");
            buff[byte_size] = '\0';
            printf("Worker %d received msg: %s\n", id, buff);
            
            //Release the semaphore
            ret = sem_post(lock);
            CHECK(ret == -1, "sem_post");

            //Convert ascii to integer and 'do' task
            sleep_durration = atoi(buff);
            printf("Worker %d running '%s'. Time requirement: %.3f\n", id, msg, ((double)sleep_durration)/1000000);
        }
    }
    mq_close(mq);
}

void producer(mqd_t mq, sem_t *count) {
    srand(time(NULL));
    char buff[MSG_SIZE];
    char *e; //Used for error checking
    int e2; //Used for error checking

    //Get a file pointer
    char* filename = "test.txt";
    FILE *file = fopen(filename, "r");
    CHECK(file == NULL, "fopen");
    //Read lines from the file and add them to the queue
    e = fgets(buff, MSG_SIZE, file);
    CHECK(e == NULL, "fgets");
    buff[strlen(buff)-1] = '\0';
    //Send to queue
    printf("Producer sending: %s\n", buff);
    e2 = mq_send(mq, buff, sizeof(buff), 0);
    CHECK(e2 == -1, "mq_send");
    //Generate random time to send 
    sprintf(buff, "%d", rand()%1000000+500000);
    printf("Producer sending %s\n", buff);
    e2 = mq_send(mq, buff, sizeof(buff), 0);
    CHECK(e2 == -1, "mq_send");
    
    sem_post(count);
}

int main(int argc, char**argv) {
    int ret;
    int pid[2];

    //Create a message queue
    struct mq_attr attr = {0, 10, MSG_SIZE, 0};
    mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    CHECK((mqd_t)-1 == mq, "mq_open");

    //Create sem lock (locks the queue)
    sem_t lock; 
    ret = sem_init(&lock, 1, 1);
    CHECK(ret == 1, "sem_init");

    //Create sem count (counts the items on the queue)
    sem_t count;
    ret = sem_init(&count, 1, 0);
    CHECK(ret == 1, "sem_init");

    //Fork and call worker
    pid[0] = fork(); 
    if (pid[0] == 0) {
        printf("Worker starting\n");
        worker(mq, &lock, &count, 1);
        printf("Worker exiting\n");
        exit(0);
    }

    // //Fork and call producer
    // pid[1] = fork();
    // if (pid[1] == 0) {
        printf("Producer starting\n");
        producer(mq, &count);
        printf("Producer sending EXIT code\n");
        ret = mq_send(mq, "EXIT", sizeof("EXIT"), 0);
        CHECK(ret == -1, "mq_send");
        sem_post(&count);
        printf("Producer exiting\n");
        // exit(0);
    // }

    for (int i = 0; i < 1; i++) {
        printf("Waiting for process pid[%d]\n", i);
        ret = waitpid(pid[i], NULL, 0);
        CHECK(ret == -1, "waitpid");
    }

    printf("All processes have exited\n");
    
    mq_close(mq);
    sem_destroy(&lock);
    sem_destroy(&count);
    mq_unlink(QUEUE_NAME);

}