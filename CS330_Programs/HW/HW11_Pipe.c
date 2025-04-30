#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <sys/random.h>
#include <wait.h>

#define MSG_SIZE 72
#define EXIT "quit"

#define CHECK(cond,msg) \
do { \
    if ((cond)) {\
        perror(msg);\
        fprintf(stderr, "ERROR: line: %d\n", __LINE__);\
        exit(1);\
    }\
} while(0)

void main(int argc, char**argv) {
    srand(time(NULL));
    char buff[MSG_SIZE];
    char *filename;
    int children;

    //Error check arguments using a bool and a stat struct
    bool valid = true;
    struct stat st;
    if (argc != 3) {
        printf("Invalide arguments\n");
        printf("Example: './a.out <filename> <thread_count>\n\n");
        exit(1);
    }

    filename = argv[1];
    children = atoi(argv[2]);

    //Check if file exists
    if (stat(filename, &st) == -1) {
        printf("Error Argument 1: %s. The file does not exist!\n", filename);
        valid = false;
    }
    //Check consumer_count (argv[2]) validity
    if (children < 0 || children > 98) {
        printf("Argument 2 must be a number between 0 and 99!\n");
        valid = false;
    }
    if (!valid) {
        exit(1);
    }

    //Open the file
    FILE *file;
    CHECK(fopen(filename, "r") == NULL, "fopen");

    //Create the pipe using a fd array
    int fd[2];
    CHECK(pipe(fd) == -1, "main's pipe(fd)");

    //Create the queue sem
    sem_t queue;
    CHECK(sem_init(&queue, 1, 0) == -1, "sem_init");

    //Create the workers sem
    sem_t workers;
    CHECK(sem_init(&workers, 1, 0) == -1, "main's sem_init(&workers, 1, 0)");

    //Create the jobs sem
    sem_t job;
    CHECK(sem_init(&job, 1, 1) == -1, "sem_init");

    pid_t pid[2];
    for(int i = 0; i < children; i++)
        pid[i] = fork();
    //If in child process
    if (getpid() == 0) {
        char* receive = "Child received %s";

        //Close Writing side of pipe. (Hint fd[1])
        close(fd[1]);

        while(true) {
            //try to get the job
            sem_wait(&job);

            //tell producer a worker is ready
            sem_post(&workers);

            //read from pipe -> buff
            read(fd[0], buff, MSG_SIZE*sizeof(char));

            //display received msg
            printf(receive, buff);

            if (strcmp(buff, EXIT)) {
                exit(0);
            }
        }
    }

    //Start writing messages to the pipe
    int msg_count = 0;
    while (fgets(buff, MSG_SIZE*sizeof(char), file) == NULL) {
        msg_count++;
        sem_wait(&workers);
        write(fd[0], buff, MSG_SIZE);
        // //Put a random time in buff
        // sprintf(buff, "%d", rand()%1000000 + 1000000);
        // write(fd[0], buff, MSG_SIZE*sizeof(char));
        usleep(rand()%1000000 + 1000000);
    }

    close(fd[0]);
    close(fd[1]); 

    for (int i = 0; i < children; i++){
        wait(NULL);
    }

    printf("%d messages sent\n", msg_count);

}