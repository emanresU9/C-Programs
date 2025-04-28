#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>
#include <printf.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <wait.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>

void producer();
void consumer();
void create_mq(mqd_t *mq);
void open_mq(mqd_t *mq);

#define QUEUE_NAME "/queue1006"
#define MAX_SIZE 20
#define MAX_MSGS 10

char *DIRECTIONS_FILE;
int P_COUNT;
sem_t* jobs;
sem_t* workers;

int main(int argc, char** argv){
    // checkargs(argv);
    DIRECTIONS_FILE = argv[1];
    P_COUNT = atoi(argv[2]);
    producer();
    return 0;
}

void producer() {
    //Seed Random Number Generator
    srand(time(NULL));
    int r; 

    printf("Running Producer\n");
    
    //Open a message queue
    mqd_t mq;
    create_mq(&mq);
    //Initialize the JOBS_SEM to 0 (no work has been added yet)
    jobs = sem_open("JOBS_SEM", O_CREAT | O_RDWR, 0600, 0);
    //Initialize the WORKERS_SEM to P_COUNT
    workers = sem_open("WORKERS_SEM", O_CREAT | O_RDWR, 0600, P_COUNT);

    //loop and fork then call consumer() in child processes
    pid_t pids[P_COUNT];
    printf("Creating %d Forks\n",P_COUNT);
    for(int i = 0; i < P_COUNT; i++) {
        if (!(pids[i] = fork())){
            consumer();
            exit(0);
        } else if (pids[i] == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        printf("Created Child %d\n", i);
    }

    //Open the file using fopen
    //Each line from the file is like a job that gets added to the queue
    char buff[MAX_SIZE];
    printf("Opening file\n");
    FILE* file = fopen(DIRECTIONS_FILE, "r");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    //Read each line into buff
    while(fgets(buff, MAX_SIZE, file)) {
        printf("%s", buff);
        // usleep(rand()%1000000 + 500000);
        //Remove from workers or wait
        sem_wait(workers);
        printf("Work added to queue");
        if ((mq_send(mq, buff, strlen(buff), 0)) == -1) {
            perror("mq_send");
            exit(EXIT_FAILURE);
        }
        //Add to jobs
        sem_post(jobs);
    }

    //Close file
    fclose(file);
    //Wait for children
    for (int i = 0; i < P_COUNT; i++) {
        sem_wait(workers); // Ensure there’s room to send
        mq_send(mq, "__EXIT__", strlen("__EXIT__") + 1, 0);
        sem_post(jobs);
    }
    for (int i = 0; i < P_COUNT; i++)
        wait(NULL);

    mq_unlink(QUEUE_NAME);
    sem_close(workers);
    sem_close(jobs);
    sem_unlink("JOBS_SEM");
    sem_unlink("WORKERS_SEM");
}
void consumer() {
    srand(time(NULL) ^ getpid());
    mqd_t mq;
    open_mq(&mq);
    char buff[MAX_SIZE];
    bool exit = false;
    while(!exit) {
        //Remove from jobs
        sem_wait(jobs);
        //pull from queue
        mq_receive(mq, buff, sizeof(buff), NULL);
        if (strcmp(buff, "__EXIT__") == 0) {
            printf("Goodbye");
            exit = true;
        }
        else {
            //Simulate job
            double sec = rand()%1000000;
            printf("Simulating %s for %f seconds\n", buff,((double)sec)/1000000 + .5);
            // usleep(sec);
        }
        //Add back to workers
        sem_post(workers);
    }
    mq_close(mq);
    sem_close(jobs);
    sem_close(workers);
}

void create_mq(mqd_t *mq) {
    struct mq_attr attr;
    attr.mq_curmsgs = 0;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    printf("Creating mqueue\n");
    *mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    if ((mqd_t)-1 == *mq) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }
} 
void open_mq(mqd_t *mq) {
    printf("Opening mqueue\n");
    *mq = mq_open(QUEUE_NAME, O_RDWR);
    if ((mqd_t)-1 == *mq) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }
} 