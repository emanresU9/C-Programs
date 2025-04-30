/**
 * name: Christopher Variara
 * course: CS330
 * date: November 26, 2023
 * homework label: HW11_Pipe.cpp
 * file location: ~variarc/www/HW11/HW11_Pipe.cpp
 * assignment: Use Linux pipes to implement
 *             a producer/consumer model with semaphores.
 * compiler: gcc
 */

#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

struct Work
{
  char description[20];
  long execution_time;
};

int numConsumers = 4;

// semaphores
sem_t *consumer_sema;
sem_t *producer_sema;

// pipe file descriptor
int pipe_fd[2];

void consumer()
{
  struct Work work;

  // Close the write end of the pipe, as consumers only read
  close(pipe_fd[1]);

  while (1)
  {
    if (sem_wait(producer_sema) == -1)
    {
      perror("Error in Consumer! sem_wait producerReady");
      exit(1);
    }

    ssize_t bytes_read = read(pipe_fd[0], (char *)&work, sizeof(struct Work));
    if (bytes_read == -1)
    {
      perror("Consumer's 'read'");
      exit(1);
    }
    else if (bytes_read == 0)
    {
      // End of file, no more data to read
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

  // Close the read end of the pipe
  close(pipe_fd[0]);
}

void producer()
{
  struct Work work;

  FILE *inputFile = fopen("input.txt", "r");
  if (inputFile == NULL)
  {
    perror("Producer's 'fopen'");
    exit(1);
  }

  if (pipe(pipe_fd) == -1)
  {
    perror("Producer's 'pipe'");
    exit(1);
  }

  // Fork consumers
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

  char producerFileSize[100];
  sprintf(producerFileSize, "producer%d_log.txt", getpid());

  // Close the read end of the pipe, as the producer only writes
  close(pipe_fd[0]);

  while (fscanf(inputFile, "%s", work.description) != EOF)
  {
    work.execution_time = (rand() % 1000000000) + 500000000;

    if (write(pipe_fd[1], (char *)&work, sizeof(struct Work)) == -1)
    {
      perror("Producer's 'write'");
      exit(1);
    }

    FILE *producerFile = fopen(producerFileSize, "a");
    fprintf(producerFile, "Producer %d Message is: %s, Execution Time: %ld\n", getpid(), work.description, work.execution_time);
    fclose(producerFile);

    printf("Producer Message is: %s, Execution Time: %ld\n", work.description, work.execution_time);

    if (sem_post(producer_sema) == -1)
    {
      perror("Error in Producer! sem_post producerReady");
      exit(1);
    }
  }

  // Close the write end of the pipe to signal consumers that no more data will be written
  close(pipe_fd[1]);

  fclose(inputFile);
}

int main()
{
  // Initialize semaphores
  consumer_sema = sem_open("/consumer_sema", O_CREAT, 0600, numConsumers);
  producer_sema = sem_open("/producer_sema", O_CREAT, 0600, 0);
  if (consumer_sema == SEM_FAILED || producer_sema == SEM_FAILED)
  {
    perror("Semaphore initialization failed");
    exit(1);
  }

  // Call producer in the main process
  producer();

  // Wait for all child processes to finish
  int waitcount = 1;
  while (wait(NULL) != -1)
  {
    printf("WAITED FOR CONSUMER %d\n", waitcount++);
  }

  // Clean up
  sem_destroy(consumer_sema);
  sem_destroy(producer_sema);

  return 0;
}
