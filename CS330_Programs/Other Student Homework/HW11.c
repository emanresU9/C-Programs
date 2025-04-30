/*
* Name:           Sawyer Davis
* Course:         CS330
* Date:           4.14.25
* Homework Label: HW11_Pipe
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#define SEM_MUTEX_NAME "/sem_mutex"
#define NUM_CONSUMERS 3
#define MAX_MSG_SIZE 256
#define INPUT_FILE "work.txt"

int pipe_fd[2];

void consumer_process(int read_fd, sem_t *sem) {
    char work_description[MAX_MSG_SIZE];
    int bytes_read;

    while (1) {
        bytes_read = read(read_fd, work_description, MAX_MSG_SIZE);
        if (bytes_read > 0) {
            work_description[bytes_read] = '\0';  
            if (strcmp(work_description, "QUIT") == 0) {
                //printf("Consumer %d exiting.\n", getpid());
                break;
            }
            printf("Consumer %d processing: %s\n", getpid(), work_description);
            sleep(1);
            printf("Consumer %d finished work and signaling semaphore.\n", getpid());
            sem_post(sem);
        }
    }
    close(read_fd); 
}
    
int main() {
    sem_t *mutex_sem;
    FILE *work_file;
    char work_description[MAX_MSG_SIZE];
    int i;

    srand(time(NULL));

    sem_unlink(SEM_MUTEX_NAME);
    mutex_sem = sem_open(SEM_MUTEX_NAME, O_CREAT, 0644, 1);
    
    if (pipe(pipe_fd) == -1) {
        perror("pipe creation failed");
        exit(1);
    }

    for (i = 0; i < NUM_CONSUMERS; i++) {
        if (fork() == 0) {  // child
            close(pipe_fd[1]);
            consumer_process(pipe_fd[0], mutex_sem);
            exit(0);
        }
    }

    close(pipe_fd[0]); 

    work_file = fopen(INPUT_FILE, "r");
    

    while (fgets(work_description, MAX_MSG_SIZE, work_file)) {
        work_description[strcspn(work_description, "\n")] = 0; 
        printf("Producer ready to send: %s\n", work_description);
        sem_wait(mutex_sem);
        if (write(pipe_fd[1], work_description, strlen(work_description) + 1) == -1) {
            perror("Failed to write to pipe");
            continue;
        }
        printf("Producer sent: %s\n", work_description);
    }

    fclose(work_file);
    for (i = 0; i < NUM_CONSUMERS; i++) {
        sem_wait(mutex_sem);
        if (write(pipe_fd[1], "QUIT", 5) == -1) {
            perror("Failed to write QUIT to pipe");
        }
    }

    close(pipe_fd[1]); 

    while( wait(NULL) != -1);

    sem_close(mutex_sem);
    sem_unlink(SEM_MUTEX_NAME);

    return 0;
}
