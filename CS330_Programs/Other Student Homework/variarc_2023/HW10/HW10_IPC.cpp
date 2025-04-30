/**
 * name: Christopher Variara
 * course: CS330
 * date: November 19, 2023
 * homework label: HW10_IPC.cpp
 * file location: ~variarc/www/HW10/HW10_IPC.cpp
 * assignment: Use Linux message queues to implement
 *             a producer/consumer model with semaphores.
 * compiler: gcc
 */

#include <sys/types.h>
#include <fcntl.h>
#include <mqueue.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

#define Q_CAPACITY 10
#define Q_NAME "/varc"
#define MY_SEM "/varc2"

// Work info
struct Work
{
  char description[20];
  long execution_time;
};

mqd_t jobQ;
// sem_t *semaphore;
sem_t *consumer_sema;
sem_t *producer_sema;
int numConsumers = 4;

const char *inputFileName = "input.txt";

void consumer()
{
  struct Work work;

  // sem_wait(producer_sema); // Wait for the semaphore

  while (1)
  {
    if (sem_wait(producer_sema) == -1)
    {
      perror("Error in Consumer! sem_wait producerReady");
      exit(1);
    }
    if (mq_receive(jobQ, (char *)&work, sizeof(struct Work), NULL) == -1)
    {
      perror("Consumer's 'mq_receive'");
      break;
    }

    time_t arrivalTime = time(NULL);

    char consumerFileSize[100];
    sprintf(consumerFileSize, "consumer%d_log.txt", getpid());
    FILE *consumerFile = fopen(consumerFileSize, "a");

    fprintf(consumerFile, "Consumer %d Received: %s, Execution Time: %ld\n", getpid(), work.description, work.execution_time);
    fclose(consumerFile);

    printf("Consumer Received: %s, Execution Time: %ld\n", work.description, work.execution_time);
    usleep(work.execution_time);

    if (sem_post(consumer_sema) == -1)
    {
      perror("Error in Consumer! sem_post consumerReady");
      exit(1);
    }
  }
}

void producer()
{
  FILE *inputFile = fopen(inputFileName, "r");
  if (inputFile == NULL)
  {
    perror("Producer's 'fopen'");
    exit(1);
  }
  
  //  Fork consumers
  for (int i = 0; i < numConsumers; i++)
  {
    if (fork() == 0)
    {
      // Child process
      consumer();
      exit(0);
    }
    else if (fork() < 0)
    {
      perror("Fork failed");
      exit(1);
    }
  }

  struct Work work;

  char producerFileSize[100];
  sprintf(producerFileSize, "producer%d_log.txt", getpid());

  while (fscanf(inputFile, "%s", work.description) != EOF)
  {
    work.execution_time = (rand() % 1000000000) + 500000000;

    time_t arrivalTime = time(NULL);

    FILE *producerFile = fopen(producerFileSize, "a");
    fprintf(producerFile, "Producer %d Message is: %s, Execution Time: %ld\n", getpid(), work.description, work.execution_time);
    fclose(producerFile);

    printf("Producer Message is: %s, Execution Time: %ld\n", work.description, work.execution_time);

    if (mq_send(jobQ, (const char *)&work, sizeof(struct Work), 0) == -1)
    {
      perror("Producer's 'mq_send'");
      exit(1);
    }

    usleep(500000); // Sleep for a short interval

    if (sem_post(producer_sema) == -1)
    {
      perror("Error in Producer! sem_post producerReady");
      exit(1);
    }
  }

  fclose(inputFile);
}

int main()
{
  // Create message queue
  struct mq_attr mqAttr;
  mqAttr.mq_flags = 0;
  mqAttr.mq_maxmsg = Q_CAPACITY;
  mqAttr.mq_msgsize = sizeof(struct Work);
  mqAttr.mq_curmsgs = 0;

  jobQ = mq_open(Q_NAME, O_CREAT | O_RDWR, 0600, &mqAttr);
  if (jobQ == (mqd_t)(-1))
  {
    perror("Main's 'mq_open'");
    exit(1);
  }

  // Open semaphore
  consumer_sema = sem_open(Q_NAME, O_CREAT, 0600, numConsumers);
  producer_sema = sem_open(Q_NAME, O_CREAT, 0600, 0);
  if (consumer_sema == SEM_FAILED || producer_sema == SEM_FAILED)
  {
    perror("Semaphore initialization failed");
    exit(1);
  }

  sem_post(consumer_sema); // Start the semaphore to allow logging
  sem_post(producer_sema);

  // Call producer in the main process
  producer();

  // Wait for all child processes to finish
  int waitcount = 1;
  while (wait(NULL) != -1)
  {
    printf("WAITED FOR CONSUMER %d\n", waitcount++);
  }

  // Clean up
  sem_close(consumer_sema);
  sem_close(producer_sema);
  sem_unlink(MY_SEM);
  mq_close(jobQ);
  mq_unlink(Q_NAME);

  return 0;
}
