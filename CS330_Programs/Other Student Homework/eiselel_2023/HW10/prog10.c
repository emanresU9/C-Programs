/*
 *   Author : Lance Eisele (eiselel)
 *   Date : 11/11/2023
 *   Assignment : HW10_Interprocess_Communication
 *   Meeting Time : 2:00pm-3:50 TR
 */
#include <fcntl.h>     //For any 'O_' constants
#include <mqueue.h>    //For message queues	(Link with -lrt)
#include <semaphore.h> //For semaphore stuff	(Link with -pthread)
#include <stdio.h>     //For printf()
#include <unistd.h>    //For fork() and sleep()
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MSG_CLAMP 20
#define SEM_ID "/LESEM"
#define QUE_ID "/LEQUE"

sem_t *semID;
mqd_t mqID;
FILE *file;
struct mq_attr mqAttr;

void message(char *filename, int consumernum);
void producer();
void consumer();

int main()
{
    printf("Starting Program\n");

    message("p10input.txt", 3);

    return 0;
}

void message(char *filename, int consumernum)
{
    file = fopen(filename, "r");

    // Open existing Semaphore
    perror("Semaphore Open");
    semID = sem_open(SEM_ID, O_RDWR, 0666, 0);
    // Check if semaphore opened
    if (semID == SEM_FAILED)
    {
        // Display error message
        perror("Main's 'sem_open'");

        // Exit back to main
        exit(1);
    }

    mqAttr.mq_maxmsg = 10;
    mqAttr.mq_msgsize = MSG_CLAMP;

    // Create process ID to prepare for forking
    pid_t pid;

    for (int i = 0; i < consumernum; i++)
    {
        fflush(stdout);
        pid = fork();
        fflush(stdout);

        // Child?
        if (pid == 0)
        {
            // Call consumer()
            consumer();

            // Kill child to avoid unnecessary looping
            exit(0);
        }

        // Parent?
        else if (pid > 0)
        {
        }

        // Did the fork fail?
        else
        {
            printf("\n\nERROR: Forked Failed\n\n");
            exit(1);
        }
    }

    producer();
    while (wait(NULL) != -1)
        ;

    // Unlink existing Semaphore
    sem_unlink(SEM_ID);
    perror("Semaphore Unlink");
    return;
}

void producer()
{
    printf("Entered Producer\n");

    mqd_t pdrQ = mq_open(QUE_ID, O_CREAT | O_WRONLY, 0644, &mqAttr);;
    char line[MSG_CLAMP];
    long wait = 0;

    // Check if message queue opened
    if (pdrQ == (mqd_t)(-1))
    {
        // Display error message
        perror("Producer's 'mq_open'");

        // Exit
        exit(1);
    }

    perror("BEFORE Producer's 'sem_wait'");
    wait = sem_wait(semID);
    perror("AFTER Producer's 'sem_wait'");

    // Error check
    if (wait < 0)
    {
        perror("Producer's 'sem_wait'");
        exit(1);
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        // line[strcspn(line, "\n")] = '\0';

        perror("BEFORE Producer's 'mq_send'");
        mq_send(pdrQ, line, strlen(line), 0);
        perror("AFTER Producer's 'mq_send'");
    }
    fclose(file);
    return;
}
void consumer()
{
    mqd_t rcvQ;
    char fileContents[MSG_CLAMP];

    // Open message queue
    rcvQ = mq_open(QUE_ID, O_CREAT | O_RDONLY, 0644, &mqAttr);
    if ((mqd_t)-1 == rcvQ)
        perror("mq_open failed in consumer");

    // Check if message queue opened
    if (rcvQ == (mqd_t)(-1))
    {
        // Display error message
        perror("Consumer's 'mq_open' failed");

        // Exit
        exit(1);
    }
    // Check if semaphore valid
    if (semID == SEM_FAILED)
    {
        // Display error message
        perror("Consumer's 'sem_open'");

        // Exit back to main
        return;
    }

    mq_receive(rcvQ, fileContents, MSG_CLAMP, NULL);
    printf("Consumer Received %s\n", fileContents);

    int post = sem_post(semID);
    if (post < 0)
    {
        perror("Consumer's 'sem_post'");
        return;
    }

    return;
}