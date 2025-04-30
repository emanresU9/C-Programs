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
int fd[2];

void consumer() {
    char buff[MSG_SIZE];
    char msg[MSG_SIZE];

    close(fd[1]);

    printf("consumer %d ready\n", getpid());

    while(1) {
        read(fd[0], buff, sizeof(buff));
        printf("Consumer %d received: %s\n", getpid(), buff);

        if (strcmp(buff, EXIT) == 0) {
            break;
        }

        sem_post(sem);
    }

    close(fd[0]);
}
void producer() {
    srand(time(NULL));
    FILE *file;
    char buff[MSG_SIZE-10];
    char msg[MSG_SIZE];

    close(fd[0]);

    file = fopen(filename, "r");
    while(fgets(buff, sizeof(buff), file) != NULL){
        if (buff[strlen(buff)-1] == '\n') {
            buff[strlen(buff)-1] = '\0';
        }
        snprintf(msg, MSG_SIZE,"%s, %d", buff, rand()%1000000 + 1000000);

        CHECK(sem_wait(sem) == -1, "producer's sem_wait");

        printf("Producer sent: %s\n", msg);
        write(fd[1], msg, MSG_SIZE);
    }
    fclose(file);

    for (int i = 0; i < num_consumers; i++) 
        write(fd[1], EXIT, sizeof(EXIT));

    close(fd[1]);
}

int main(int argc, char**argv) {
    //Overwrite arguments for testing purposes
    argc = 3;
    char *fauxArgv[] = {"Run", "work.txt", "2"};
    argv = fauxArgv;

    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_msgsize = MSG_SIZE,
        .mq_maxmsg = MAX_SIZE,
        .mq_curmsgs = 0
    };

    filename = argv[1];
    num_consumers = atoi(argv[2]);

    printf("Starting Program\n");

    pipe(fd);

    sem = sem_open(SEM_NAME, O_CREAT | O_RDWR, 0666, num_consumers);

    sem_unlink(SEM_NAME);
    
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
    sem_unlink(SEM_NAME);

    printf("done\n");
    return 0;
}