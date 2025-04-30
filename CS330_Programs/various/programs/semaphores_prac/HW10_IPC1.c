#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/random.h>
#include <time.h>
#include <string.h>

char* filename;

#define CHECK(cond, ermsg) \
do { \
    if ((cond)) { \
        fprintf(stderr, "ERROR: %s, in file %s at line %d\n", \
        ermsg, __FILE__, __LINE__); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

void producer(sem_t* workers, mqd_t mq){
    srand(time(NULL));
    FILE* work_file = fopen(filename, "r");
    char instruction[20+1];
    char* s = fgets(instruction, sizeof(instruction), work_file);
    while(s != NULL) {
        usleep(rand()%1000000+500000);
        if(sem_trywait(workers) == -1) {
            
        }
        mq_send(mq, instruction, sizeof(instruction), 0);
        s = fgets(instruction, sizeof(instruction), work_file);
    }
}
void consumer(mqd_t mq){
    //Implement
}
int main(int argc, char**argv){
    //Get user input
    char *myArgv[3] = {"Run", "test.txt", "4"};
    argc = 2;
    argv = myArgv;
    filename = argv[1];

    //Create the sem and the queue
    sem_t workers;
    sem_init(&workers, 1, 0);
    struct mq_attr attr = {0, 10, 20, 0};
    mqd_t mq = mq_open("/my_queue", O_CREAT | O_RDWR, 0644, &attr);
    CHECK((mqd_t)-1 == mq, "open_mq");

    //Fork -> consumer | producer
    int pid = fork();
    if (pid == -1) {
        CHECK(1,  "fork");
    } else if (pid == 0) {
        printf("Starting consumer\n");
        mq_close(mq);
    } else {
        producer(&workers, mq);
        mq_close(mq);
    }
    sem_destroy(&workers);
}