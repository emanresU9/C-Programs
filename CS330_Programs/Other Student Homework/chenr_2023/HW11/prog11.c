//Name: Ricky Chen
//Course: CS330
//Date:
//Homework Label: HW11_Pipe


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_FILESIZE 1000000
#define MAX_SIZE 20
#define MY_SEM "/chenr-sem"
#define Q_CAPACITY 10

void producer(char *fileName, int pipe_fd) {
    printf("PRODUCER\n");
    char fileContent[MAX_FILESIZE];
    FILE *file = fopen(fileName, "r");

    if (file == NULL) {
        perror("File not opened");
        exit(1);
    }

    while (fscanf(file, "%s", fileContent) != EOF) {
        char word[MAX_FILESIZE];
        strcpy(word, fileContent);

        // Get random time
        long executionTime = ((random() % 1000000000) + 100000000);
        size_t size = MAX_FILESIZE + 1 - strlen(fileContent);
        snprintf((fileContent + strlen(fileContent) + 1), size, "%ld", executionTime);

        printf("Producer Message Length is: %ld\n", strlen(word));
        printf("Producer Message is: %s\n", word);

        // Write to the pipe
        if (write(pipe_fd, word, strlen(word) + 1) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    fclose(file);
}

void consumer(int pipe_fd) {
    printf("CONSUMER\n");
    char fileContent[MAX_FILESIZE];

    // Read from the pipe
    if (read(pipe_fd, fileContent, MAX_FILESIZE) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    printf("Received %s\n", fileContent);
}

int main(int argc, char *argv[]) {
    int temp;
    int pipe_fd[2];

    if (argc != 3) {
        printf("Wrong number of arguments entered!\n");
        return -1;
    }

    // Making sure it's not being used
    sem_unlink(MY_SEM);

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Get arguments
    int consumerCnt = atoi(argv[2]);
    char *fileName = argv[1];
    pid_t frk;

    for (int i = 0; i < consumerCnt; i++) {
        frk = fork();

        if (frk == 0) {
            // Close write end of the pipe in the child process
            close(pipe_fd[1]);
            consumer(pipe_fd[0]);

            // Kill child
            exit(0);
        } else if (frk > 0) {
            temp++;
        } else {
            perror("Fork failed");
            exit(1);
        }
    }

    // Close read end of the pipe in the parent process
    close(pipe_fd[0]);
    producer(fileName, pipe_fd[1]);

    while (waitpid(-1, NULL, 0) > 0)
        printf("Waiting for child process\n");

    // Close write end of the pipe in the parent process
    close(pipe_fd[1]);

    sem_unlink(MY_SEM);

    return 0;
}
