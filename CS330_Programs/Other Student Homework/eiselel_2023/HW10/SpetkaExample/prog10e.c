/*
Example Producer Consumer using Message Queues
based on week12/pipexyx.c and week14 examples in
https://www.cs.sunyit.edu/~scott/classes/CS330/
Scott Spetka updates to:

scott@gaben0:~/www/classes/CS330$ fg
vi week14/procon.c

[1]+  Stopped                 vi week14/procon.c
scott@gaben0:~/www/classes/CS330$ pwd
/home/faculty/scott/www/classes/CS330
scott@gaben0:~/www/classes/CS330$ ls -l week12/pipexyx.c
-rw-r--r-- 1 scott faculty 4733 Apr 16  2020 week12/pipexyx.c
scott@gaben0:~/www/classes/CS330$

Program to use message queues to implement a producer / consumer model
	using a single producer process and
	a set of consumer processes
	which are forked by the producer
Original code Written by: Solomon Stevens
SUNY Politechnic Institute: CS 330
DATE: 4/23/2021
LABEL: HW10_IPC
*/

#include <sys/types.h>
#include <sys/wait.h>

#include <fcntl.h>		//For any 'O_' constants
#include <mqueue.h>		//For message queues	(Link with -lrt)
#include <semaphore.h>		//For semaphore stuff	(Link with -pthread)
#include <stdio.h>		//For printf()
#include <stdlib.h>		//For atoi() and exit()
#include <string.h>		//For strncmp() and strlen()
#include <sys/stat.h>		//For mode constants
#include <sys/types.h>		//For fork()
#include <time.h>		//For nanosleep()
#include <unistd.h>		//For fork()

#define MAX_SIZE 20		//Max size of message in bytes
#define MY_SEM "/xcott2"
#define Q_CAPACITY 10		//How many messages can fit in queue
#define Q_NAME "/xcott"

#define MSGSES

sem_t * semID;
int childnumber=1;

//Producer Function
void producer(char* fileName)
{
printf("PRODUCER\n");	//TEST*******************************************************************
	//Local variables
	char exeTyme[10];
	char fileContents[MAX_SIZE];
	FILE* inputFile;
	int send;
	long wait = 0, exeTime = 0;
	mqd_t jobQ;
	//sem_t * semID;
	size_t wrkSize;
	struct mq_attr mqAttr;

	//Open file
	perror("Before Producer's 'fopen'");
	inputFile = fopen(fileName, "r");
	perror("After Producer's 'fopen'");

	//Check if file opened
	if(inputFile == NULL)
	{
		//Display error message
		perror("Producer's 'fopen'");

		//Exit
		exit(1);
	}


	//Create message queue
	//-> Assign message queue attributes
	mqAttr.mq_flags = 0;		//Ignored in mq_open()
	mqAttr.mq_maxmsg = Q_CAPACITY;
	mqAttr.mq_msgsize = MAX_SIZE;
	mqAttr.mq_curmsgs = 0;		//Also ignored in mq_open()

#ifdef MSGSES
    /* open the mail queue */
    //mq = mq_open(QUEUE_NAME, O_WRONLY);
    jobQ = mq_open(Q_NAME, O_CREAT | O_WRONLY, 0644, &mqAttr);
    if((mqd_t)-1 == jobQ)
	perror("mq_open failed in producer");
#endif // MSGSES


	//-> Actually create message queue
	//jobQ = mq_open(Q_NAME, O_CREAT | O_RDWR, 0600, &mqAttr);
	//Check if message queue opened
	if(jobQ == (mqd_t)(-1))
	{
		//Display error message
		perror("Producer's 'mq_open'");

		//Exit
		exit(1);
	}


#ifdef SES
	//Open semaphore
	//semID = sem_open(MY_SEM, O_CREAT, 0600, 0);
	semID = sem_open(MY_SEM, O_CREAT, 0600, 0);

	//Check if semaphore opened
	if(semID == SEM_FAILED)
	{
		//Display error message
		perror("Producer's 'sem_open'");

		//Exit back to main
		exit(1);
	}
	perror("BEFORE Producer's 'sem_wait'");
#endif // SES

	//Try to enter critical region
	perror("BEFORE Producer's 'sem_wait'");
	wait = sem_wait(semID);
	perror("AFTER Producer's 'sem_wait'");

	//Error check
	if(wait < 0)
	{
		perror("Producer's 'sem_wait'");
		exit(1);
	}

	if(wait == 0)
	{
		printf("Waiting...\n");
	}

	//Communicate to consumer what to do
	//-put it in message queue (mq_send())
	//-Need work message && random time interval

	while(fscanf(inputFile, "%s", fileContents) != EOF)
	{
		char myString[100];
		strcpy(myString, fileContents);

		wrkSize = strlen(fileContents);
		fileContents[wrkSize] = ',';


		//Get random() time interval
		exeTime = ((random() % 1000000000) + 500000000);	//should be between 0.5 and 1.5 seconds

		//Append time onto 'fileContents'
		size_t t3st = MAX_SIZE + 1 - wrkSize;
		snprintf((fileContents + wrkSize + 1), t3st, "%ld", exeTime);

		printf("Producer Message Length is: %ld\n",strlen(myString));
		printf("Producer Message is: %s\n",myString);
		perror("Before Producer's 'mq_send'");
		//Put in message queue
		//send = mq_send(jobQ, fileContents, strlen(fileContents), 0);
		send = mq_send(jobQ, myString, strlen(myString), 0);
		perror("After Producer's 'mq_send'");

		//Check if message queue was sent
		if(send == -1)
		{
			perror("Producer's 'mq_send'");
			exit(1);
		}

	}

	//Close file
	fclose(inputFile);

	//Exit back to main
	return;
}


//Consumer Function
void consumer(void)
{
printf("CONSUMER\n");	//TEST************************************************************************
	//Local variables
	char fileContents[MAX_SIZE];
	int post;
	mqd_t jobQ;
	sem_t * semID;

#ifdef MSGSES
    /* initialize the queue attributes */
    //attr.mq_flags = 0;
    //attr.mq_maxmsg = 10;
    //attr.mq_msgsize = MAX_SIZE;
    //attr.mq_curmsgs = 0;
	struct mq_attr mqAttr;
	mqAttr.mq_flags = 0;		//Ignored in mq_open()
	mqAttr.mq_maxmsg = Q_CAPACITY;
	mqAttr.mq_msgsize = MAX_SIZE;
	mqAttr.mq_curmsgs = 0;		//Also ignored in mq_open()

    /* create the message queue */
    jobQ = mq_open(Q_NAME, O_CREAT | O_RDONLY, 0644, &mqAttr);
    if((mqd_t)-1 == jobQ)
	perror("mq_open failed in consumer");
#endif // MSGSES

	//Open message queue
	//jobQ = mq_open(Q_NAME, O_RDONLY);

	//Check if message queue opened
	if(jobQ == (mqd_t)(-1))
	{
		//Display error message
		perror("Consumer's 'mq_open' failed");

		//Exit
		exit(1);
	}

#ifdef SES
	//Open semaphore
	semID = sem_open(MY_SEM, O_CREAT, 0600, 0);

	//Check if semaphore opened
	if(semID == SEM_FAILED)
	{
		//Display error message
		perror("Consumer's 'sem_open'");

		//Exit back to main
		return;
	}
#endif // SES

	//Receive instructions from producer
	//-using message queues (mq_receive())
	//-Loop here
printf("Receive\n");
	mq_receive(jobQ, fileContents, MAX_SIZE, NULL);
//printf("Received\n");
printf("Consumer %d Received %s\n", childnumber, fileContents);

	//Unlock semaphore
	post = sem_post(semID);

	//Error checking
	if(post < 0)
	{
		perror("Consumer's 'sem_post'");
		return;
	}

	//Exit back to main
	return;
}

//Main Function
int main(int argc, char**argv)
{
	//Error Checking before we do anything
	perror("MAIN ");

	//Check to see if given correct number of arguments
	if(argc != 3)
	{
		printf("\nERROR: Incorrect number of arguments\n");
		printf("SYNTAX: 'Run' , 'FileToOpen' , 'NumConsumers'\n\n");
		return 1;
	}

	//if (mq_unlink(Q_NAME)); // ignore return values
	//if (sem_unlink(MY_SEM)); // ignore return values
	perror("MAIN After unlink ");

	sleep(1);

	//Open semaphore
	//semID = sem_open(MY_SEM, O_CREAT, 0600, 0);
        // Set to 1 so the producer can down it
	//semID = sem_open(MY_SEM, O_CREAT, 0660, 1);
	//semID = sem_open(MY_SEM, O_RDWR, 0666, 1);
	semID = sem_open(MY_SEM,O_RDWR, 0666, 2);

	//Check if semaphore opened
	if(semID == SEM_FAILED)
	{
		//Display error message
		perror("Main's 'sem_open'");

		//Exit back to main
		exit(1);
	}

	perror("MAIN After Sem ");
	//Local variables
	int i = 0;
	int numConsumers = atoi(argv[2]);
	char* fileName = argv[1];
	pid_t frk;

printf("NumConsumers: %d\nFileName: %s\n", numConsumers, fileName);

	//Loop through 'numConsumers' times to fork
	for(i = 0; i < numConsumers; i++)
	{
		//Fork
		frk = fork();

		//Child?
		if(frk == 0)
		{
			//Call consumer()
			consumer();

			//Kill child to avoid unnecessary looping
			exit(0);
		}

		//Parent?
		else if(frk > 0)
		{
			childnumber++;
			//Parent, Do Nothing.
		}

		//Did the fork fail?
		else
		{
			printf("\n\nERROR: Forked Failed To Open\n\n");
			exit(1);
		}

	}

	//Call producer() (We only need one producer, hence outside the loop)
	producer(fileName);

	int waitcount=1;
	while (wait(NULL) != -1)
	printf("WAITED FOR CONSUMER %d ",waitcount++);

	//Exit
	return 0;
}
