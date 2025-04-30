/*
 *   Author : Lance Eisele (eiselel)
 *   Date : 11/11/2023
 *   Assignment : HW10_Interprocess_Communication
 *   Meeting Time : 2:00pm-3:50 TR
 */

// C program to implement pipe in Linux
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include <semaphore.h>
#include <fcntl.h> // Pipe and Semaphore

#define SEM_ID "/LESEM"
#define MSG_STOP "exit"
#define MSG_CLAMP 20

sem_t *msgsem;
FILE *file;
char pipebuff[MSG_CLAMP];
int rw[2];

void message(char *filename, int consumernum);
void producer();
void consumer();

int main()
{
    message("p11input.txt", 3);
}

void message(char *filename, int consumernum)
{
    if (pipe(rw) == -1)
    {
        perror("pipe"); // error in pipe
        exit(1);        // exit from the program
    }
    // sem_init(msgsem,1,0);
    msgsem = sem_open(SEM_ID, O_RDWR, 0666, 0);

    file = fopen(filename, "r");
    for (int i = 0; i < consumernum; i++)
    {
        fflush(stdout);
        pid_t pid = fork();
        fflush(stdout);

        if (pid == 0)
        {
            // child
            consumer();
            exit(0);
        }
    }

    producer();

    while (wait(NULL) > 0)
        ;

    sem_destroy(msgsem);

    fclose(file);
    return;
}

void producer()
{
    char line[MSG_CLAMP];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        line[strcspn(line, "\n")] = '\0';
        write(rw[1], line, MSG_CLAMP);
        int wait = sem_wait(msgsem);
    }
    sleep(1);
}
void consumer()
{
    sem_post(msgsem);
    read(rw[0], pipebuff, MSG_CLAMP);
    printf("%s\n", pipebuff);
}