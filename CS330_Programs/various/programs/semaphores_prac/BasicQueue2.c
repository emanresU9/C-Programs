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
#include <wait.h>

#define QUEUE_NAME "/job_queue"
#define MAX_SIZE 256
#define NUM_JOBS 10
#define NUM_WORKERS 4

sem_t sem;

void create_jobs(mqd_t mq) {
    char job[MAX_SIZE];
    for (int i = 0; i < NUM_JOBS; i++) {
        snprintf(job, sizeof(job), "Job %d", i);
        if (mq_send(mq, job, sizeof(job), 0) == -1) {
            perror("mq_send");
            exit(EXIT_FAILURE);
        }
        printf("Scheduler: Sent %s\n", job);
        sleep(1);
    }
    // Send shutdown signal to each worker
    for (int i = 0; i < NUM_WORKERS; ++i) {
        snprintf(job, MAX_SIZE, "EXIT");
        mq_send(mq, job, strlen(job) + 1, 0);
    }
}

void worker(mqd_t mq, int id) {
    char buffer[MAX_SIZE];
    while (1) {
        sem_wait(&sem);
        ssize_t bytes_read = mq_receive(mq, buffer, sizeof(buffer), NULL);
        if (bytes_read >= 0) {
            sem_post(&sem); // Unlock
            buffer[bytes_read] = '\0';
            if (strcmp(buffer, "EXIT") == 0) {
                printf("Worker %d: Received shutdown signal.\n", id);
                break;
            }
            printf("Worker %d: Received %s\n", id, buffer);
            int delay = rand() % 4 + 1; // 1-4 seconds
            sleep(delay);
            printf("Worker %d: Processed %s in %d sec\n", id, buffer, delay);
        }
    }
    exit(0);
}

int main() {
    srand(time(NULL));

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    if (mq == -1) {
        perror("mq_open");
        exit(1);
    }

    // Initialize semaphore
    sem_init(&sem, 1, 1); // Shared between processes

    pid_t workers[NUM_WORKERS];
    for (int i = 0; i < NUM_WORKERS; ++i) {
        workers[i] = fork();
        if (workers[i] == 0) {
            worker(mq, i + 1);
        }
    }

    create_jobs(mq);

    // Wait for queue to empty
    sleep(5);

    // Cleanup: Wait for workers and remove queue
    for (int i = 0; i < NUM_WORKERS; ++i)
        waitpid(workers[i], NULL, 0);

    mq_close(mq);
    mq_unlink(QUEUE_NAME);
    sem_destroy(&sem);

    printf("Scheduler: All jobs completed.\n");
    return 0;
}