#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <mqueue.h>
#include <fcntl.h>
#include <string.h>
#include <sys/random.h>
#include <sys/wait.h>

#define CHECK(condition, msg) \
    do { \
        if ((condition)) { \
            perror(msg); \
            fprintf(stderr, "ERROR: line: %d\n", __LINE__); \
        } \
    } while(0)

#define MSG_SIZE 100
#define MAX_SIZE 10
#define EXIT "byebye"
#define SEM_NAME "/sem1234"
#define MQ_NAME "/mq1234"

char* filename;
int num_consumers;
sem_t *sem;
mqd_t mq;

void consumer() {
    char buff[MSG_SIZE];
    char msg[MSG_SIZE];
    printf("consumer %d ready\n", getpid());

    while(1) {
        CHECK(mq_receive(mq, buff, sizeof(buff), 0) == -1, "Consumer's mq_receive");
        printf("Consumer %d received: %s\n", getpid(), buff);

        if (strcmp(buff, EXIT) == 0) {
            break;
        }

        sem_post(sem);
    }
}
void producer() {
    srand(time(NULL));
    FILE *file;
    char buff[MSG_SIZE-10];
    char msg[MSG_SIZE];

    file = fopen(filename, "r");
    while(fgets(buff, sizeof(buff), file) != NULL){
        if (buff[strlen(buff)-1] == '\n') {
            buff[strlen(buff)-1] = '\0';
        }
        snprintf(msg, MSG_SIZE,"%s, %d", buff, rand()%1000000 + 1000000);

        CHECK(sem_wait(sem) == -1, "producer's sem_wait");
        printf("Producer sent: %s\n", msg);
        mq_send(mq, msg, MSG_SIZE, 0);
    }
    fclose(file);

    for (int i = 0; i < num_consumers; i++) 
        mq_send(mq, EXIT, sizeof(EXIT), 0);
}

int main(int argc, char**argv){
    // //Overwrite arguments for testing purposes
    // argc = 3;
    // char *fauxArgv[] = {"Run", "work.txt", "2"};
    // argv = fauxArgv;

    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_msgsize = MSG_SIZE,
        .mq_maxmsg = MAX_SIZE,
        .mq_curmsgs = 0
    };

    filename = argv[1];
    num_consumers = atoi(argv[2]);

    printf("Starting Program\n");

    sem = sem_open(SEM_NAME, O_CREAT | O_RDWR, 0666, num_consumers);
    mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, 0644, &attr);

    sem_unlink(SEM_NAME);
    mq_unlink(MQ_NAME);
    
    for(int i = 0; i < num_consumers; i++) {
        if(!fork()){
            consumer();
            exit(0);
        }
    }

    producer();

    for(int i = 0; i < num_consumers; i++) {
        wait(NULL);
    }

    sem_close(sem);
    mq_close(mq);
    sem_unlink(SEM_NAME);
    mq_unlink(MQ_NAME);

    printf("done\n");
    return 0;
}