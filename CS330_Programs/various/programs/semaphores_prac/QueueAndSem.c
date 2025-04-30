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
#include <sys/wait.h>

#define MSG_SIZE 20
#define CHECK(cond, msg) \
    do {\
        if (cond) {\
            fprintf(stderr, "Error: %s (in file: %s at line %d)\n", msg, __FILE__, __LINE__);\
            exit(EXIT_FAILURE);\
        }\
    } while (0)

void producer(mqd_t mq, sem_t *count){
    char *test_data[10] = {
        "proc1",
        "proc2",
        "proc3",
        "proc4",
        "proc5",
        "proc6",
        "proc7",
        "proc8",
        "proc9",
        "proc10"
    };
    for (int i = 0; i < 10; i++) {
        printf("producer sending: %s\n", test_data[i]);
        CHECK(mq_send(mq, test_data[i], sizeof(test_data[i]), 0) == -1, "mq_send");
        sem_post(count);
    }
}
void consumer(mqd_t mq, sem_t *lock, sem_t *count) {
    char buff[MSG_SIZE+1];
    sem_wait(count);
    sem_wait(lock);
    CHECK(mq_receive(mq, buff, sizeof(buff), NULL) == -1, "mq_receive");
    buff[MSG_SIZE] = '\0';
    printf("consumer received: %s\n", buff);
    sem_post(lock);
}

int main() {
    mqd_t mq = mq_open("/my_queue", O_CREAT | O_RDWR, 0644, NULL);
    CHECK(mq == (mqd_t)-1, "mq_open");
    sem_t lock;
    sem_init(&lock, 1, 1);
    sem_t count;
    sem_init(&count , 1, 0);

    producer(mq, &count);

    for(int i = 0; i < 10; i++) {
        consumer(mq, &lock, &count);
    }
}