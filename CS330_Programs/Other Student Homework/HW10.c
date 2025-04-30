/*
* Name:           Sawyer Davis
* Course:         CS330
* Date:           4.7.25
* Homework Label: HW10_IPC
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#define SEM_MUTEX_NAME "/sem_mutex"
#define MQ_NAME "/mq_name"
#define NUM_CONSUMERS 3
#define MAX_MSG_SIZE 256
#define INPUT_FILE "work.txt" 

void consumer_process(mqd_t mq, sem_t *sem) {
    char work_description[MAX_MSG_SIZE];
    while (1) {
        if (mq_receive(mq, work_description, MAX_MSG_SIZE, NULL) > 0) {
            if (strcmp(work_description, "QUIT") == 0) {
                printf("Consumer %d exiting.\n", getpid());
                break;
            }
            printf("Consumer %d processing: %s\n", getpid(), work_description);
            sleep(rand() % 5 + 1);
            printf("Consumer %d finished work and signaling semaphore.\n", getpid());
            sem_post(sem);
        }
    }
}

int main() {
    sem_t *mutex_sem;
    mqd_t mq;
    struct mq_attr attr;
    char work_description[MAX_MSG_SIZE];
    FILE *work_file;
    int i;

    sem_unlink(SEM_MUTEX_NAME); // just in case

    mutex_sem = sem_open(SEM_MUTEX_NAME, O_CREAT, 0644, NUM_CONSUMERS);
    if (mutex_sem == SEM_FAILED) {
        perror("sem_open producer");
        exit(1);
    }

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, 0644, &attr);
    if (mq == -1) {
        perror("mq_open producer");
        exit(1);
    }

    for (i = 0; i < NUM_CONSUMERS; i++) {
        if (fork() == 0) {  // Child process
            consumer_process(mq, mutex_sem);
            exit(0);
        }
    }

    work_file = fopen(INPUT_FILE, "r");
    if (!work_file) {
        perror("Failed to open file");
        exit(1);
    }

    while (fgets(work_description, MAX_MSG_SIZE, work_file)) {
        work_description[strcspn(work_description, "\n")] = 0; 
        printf("Producer ready to send: %s\n", work_description);
        sem_wait(mutex_sem);
        if (mq_send(mq, work_description, strlen(work_description) + 1, 0) == -1) {
            perror("Failed to send message to mq");
            continue;
        }
        printf("Producer sent: %s\n", work_description);
    }

    fclose(work_file);
    for (i = 0; i < NUM_CONSUMERS; i++) {
        mq_send(mq, "QUIT", 5, 0);
    }

    for (i = 0; i < NUM_CONSUMERS; i++) {
        wait(NULL);
    }

    // cleanup time
    mq_close(mq);
    mq_unlink(MQ_NAME);
    sem_close(mutex_sem);
    sem_unlink(SEM_MUTEX_NAME);

    return 0;
}