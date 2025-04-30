//Name: Ricky Chen
//Course: CS330
//Date:
//Homework Label: HW10_IPC


#include <sys/types.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <mqueue.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAX_FILESIZE 1000000
#define MAX_SIZE 20	
#define MY_SEM1 "/chenr-sem"
#define MY_SEM2 "/wangcc-sem"
#define Q_CAPACITY 10
#define Q_NAME "/chenr"

struct mq_attr mqAttr;

void producer(char* fileName) {
    printf ("PRODUCER\n");
    char fileContent[MAX_FILESIZE];
    FILE* file = fopen(fileName, "r");

    if (file == NULL) {
        printf("File not opened");
        exit(1);
    }

    //Sets attributes
    mqAttr.mq_flags = 0;
	mqAttr.mq_maxmsg = Q_CAPACITY;
	mqAttr.mq_msgsize = MAX_SIZE;
	mqAttr.mq_curmsgs = 0;
    //Opens message queue

    mqd_t mq = mq_open(Q_NAME, O_CREAT | O_WRONLY, 0644, &mqAttr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    //Opens semaphore
    sem_t *workers = sem_open(MY_SEM1, O_CREAT, 0600, 0);
    sem_t *jobs = sem_open(MY_SEM2, O_CREAT, 0600, 0);
	if(workers == SEM_FAILED || jobs == SEM_FAILED)
	{
		perror("sem_open");
		exit(1);
	}

    while (fscanf(file, "%s", fileContent) != EOF) {
        char word[MAX_FILESIZE];
        strcpy(word, fileContent);

        //Get random time
		long executionTime = ((random() % 1000000000) + 100000000);
		size_t size = MAX_FILESIZE + 1 - strlen(fileContent);
		snprintf((fileContent + strlen(fileContent) + 1), size, "%ld", executionTime);

        printf("Producer Message Length is: %ld\n",strlen(word));
		printf("Producer Message is: %s\n",word);

        while(sem_trywait(workers) == -1) {
            usleep(100000);
        }

        if (mq_send(mq, word, strlen(word) + 1, 1) == -1) {
            perror("mq_send");
            exit(EXIT_FAILURE);
        }

        sem_post(jobs);

    }

    fclose(file);
    sem_close(jobs);
    sem_close(workers);
    return;
}

void consumer() {
    printf("CONSUMER\n");
    char fileContent[MAX_FILESIZE];
	mqd_t jobQ;

	mqAttr.mq_flags = 0;
	mqAttr.mq_maxmsg = Q_CAPACITY;
	mqAttr.mq_msgsize = MAX_SIZE;
	mqAttr.mq_curmsgs = 0;

    mqd_t rQ = mq_open(Q_NAME, O_CREAT | O_RDONLY, 0644, &mqAttr);
    if(rQ == (mqd_t)-1)
	perror("mq_open failed in consumer");

    sem_t* workers = sem_open(MY_SEM1, O_CREAT, 0600, 0);
    sem_t* jobs = sem_open(MY_SEM2, O_CREAT, 0600, 0);
	if(workers == SEM_FAILED || jobs == SEM_FAILED)
	{
		perror("sem_open");
		return;
    }

    sem_post(workers);
    while (sem_trywait(jobs) == -1) {
        usleep(1000);
    }

    printf("Receiving\n");
	mq_receive(rQ, fileContent, MAX_FILESIZE, NULL);
    printf("Received %s\n", fileContent);

	return;
}


int main(int argc, char *argv[]) {
    int temp;

    if (argc != 3) {
        printf("Wrong number of arguments entered!\n");
        return -1;
    }

    //Making sure it's not being used
    mq_unlink(Q_NAME);
    sem_unlink(MY_SEM1);

    sem_t *semID = sem_open(MY_SEM1, O_CREAT | O_EXCL, 0600, 0);
    if (semID == SEM_FAILED) {
        perror("sem_open producer");
        exit(1);
    }


    //Get arguments
    int consumerCnt = atoi(argv[2]);
    char* fileName = argv[1];
    pid_t frk;

    for (int i = 0; i < consumerCnt; i++){
        frk = fork();
        
        if (frk == 0) {
            consumer();

            //Kill child
            exit(0);
        } else if (frk > 0) {
            temp++;
        } else {
            printf("Fork failed \n");
            exit(1);
        }
    }

    producer(fileName);

    while (waitpid(-1, NULL, 0) > 0)
        printf("Waiting for child process\n");
        
    sem_close(semID);
    sem_unlink(MY_SEM1);
    mq_unlink(Q_NAME);

    return 0;
}
