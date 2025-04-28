#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>  //For sem
#include <fcntl.h>      //For sem, mqueue
#include <sys/stat.h>   //For sem, mqueue
#include <mqueue.h>     //For mqueue
#include <string.h>     //for strcpy
#include <stdbool.h>    //for bool datatypes
#include <unistd.h>     //for fork
#include <wait.h>       //to reunite the forks
#include <sys/stat.h>   //for file checking
#include <sys/random.h> //for random number generation
#include <time.h>       //for randome number generation

#define CHECK(condition, location_msg) \
do { \
    if ((condition)) { \
        perror(location_msg); \
        fprintf(stderr, "ERROR: line %d\n", __LINE__); \
    } \
} while(0)

#define WORKERS_SEM "/sem123"
#define JOBS_SEM "/sem234"
#define MESS_QUEUE "/mq1010"
#define MSG_SIZE 100
#define MAX_MSG 10
#define EXIT "KILL"

struct mq_attr qAttr;

void producer(int producerID ,char *filename, int consumer_count) {
    sem_t *workers;
    sem_t *jobs;
    mqd_t mq;
    FILE *file;
    char line[MSG_SIZE];
    char msg[MSG_SIZE];
    int t;
    int ret;
    srand(time(NULL));

    //open sem
    workers = sem_open(WORKERS_SEM, O_CREAT | O_RDWR, 0666, 0);
    CHECK(workers == SEM_FAILED, "sem_open in the producer");

    //open sem
    jobs = sem_open(JOBS_SEM, O_CREAT | O_RDWR, 0666, 0);
    CHECK(jobs == SEM_FAILED, "sem_open in the producer");

    //open queue
    mq = mq_open(MESS_QUEUE, O_CREAT | O_WRONLY, 0644, &qAttr);
    CHECK(((mqd_t)-1 == mq), "mq_open");

    //OPEN FILE
    file = fopen(filename, "r");
    CHECK(NULL == file, "producer's fopen");

    //WHILE read each line
    while(fgets(line, sizeof(line), file) != NULL) {
        //remove '\n'
        line[strlen(line) -1] = '\0';

        // wait a random amount of time.
        usleep(rand()%1000000 + 1000000);

        // COPY line into msg
        strcpy(msg, line);

        // replace the null character in msg with a comma
        msg[strlen(line)] = ',';

        // Find the execution time for the process.
        t = rand()%1000000 + 1000000;

        // append a the formated time to msg
        snprintf((msg+strlen(line)+1), MSG_SIZE-strlen(line), "%d", t);

        // decrement the workers sem
        sem_wait(workers);

        printf("Producer %-2d | Sending : %-26s\n", producerID, msg);

        // send msg on the queue
        ret = mq_send(mq, msg, sizeof(msg),0);
        CHECK(ret == -1, "producer's mq_send");

        // Tell workers that a job is ready
        sem_post(jobs);

    }//END WHILE

    //Send EXIT to each worker
    for (int i = 0; i < consumer_count; i++) {
        //Wait until worker is ready
        sem_wait(workers);
        //Add message to queue
        printf("Producer %-2d | %-8s: %-26s\n", producerID,"sending", EXIT);
        mq_send(mq, EXIT, sizeof(EXIT), 0);
        //Tell worker that job is ready
        sem_post(jobs);
        //Wait for the fork join
        wait(NULL);
    }

    //Close the sems and queue
    sem_close(workers);
    sem_close(jobs);
    mq_close(mq);
}

void consumer(int consumerID) {
    sem_t *workers;
    sem_t *jobs;
    mqd_t mq;
    int res;
    char buff[MSG_SIZE];
    char str[MSG_SIZE];
    char exeTime[11]; 
    char* pos;
    int idx;

    //Open sem
    workers = sem_open(WORKERS_SEM, O_CREAT | O_RDWR, 0666, 0);
    CHECK(workers == SEM_FAILED, "consumer's 'sem_open'");

    //Open sem
    jobs = sem_open(JOBS_SEM, O_CREAT | O_RDWR, 0666, 0);
    CHECK(jobs == SEM_FAILED, "consumer's 'sem_open'");

    //Open message queue
    mq = mq_open(MESS_QUEUE, O_CREAT | O_RDONLY, 0644, &qAttr);
    CHECK((mqd_t)-1 == mq, "consumer's mq_open");

    printf("consumer %-2d | Ready\n", consumerID);

    while(1) {
        //Signal producer that a worker is ready
        res = sem_post(workers);
        CHECK(res == -1, "consumer's mq_post");

        //Wait for producer to send a job
        res = sem_wait(jobs);
        CHECK(res == -1, "consumer's mq_post");

        //pull from queue -> str
        res = mq_receive(mq, buff, sizeof(buff), 0); // fill buff
        CHECK((res == -1), "consumer's mq_receive");    // check for error
        strcpy(str, buff);  // copy buff into str

        //Find the index of a comma character in str
        pos = str;
        while(*pos != ',' && pos != str+MSG_SIZE) 
            pos++;
        if (pos == str+MSG_SIZE) {
            if (strcmp(str, EXIT) == 0)
                printf("Consumer %-2d | Received: %-26s\n", consumerID, EXIT);
            else
                printf("Consumer %-2d | Received: Invalid Message resulting in Exit\n", consumerID);
            break;
        } 
        else
            idx = (int)(pos - str);

        //extract portion of str after comma -> exection-time
        res = snprintf(exeTime, sizeof(exeTime), "%s", str+idx+1);
        CHECK(res < 0, "consumer's snprintf");

        //remove portion of str after coma
        str[idx] = '\0';

        //Print consumer received-job message
        printf("consumer %-2d | Received: %-26s\n", consumerID, str);

        //Sleep
        usleep(atoi(exeTime));

        //Print concumer finished-job message
        printf("consumer %-2d | Finished: %-26s | cpu_time: %.4f\n", consumerID, str, (double)(atoi(exeTime))/1000000);

        fflush(stdout); 
    }
    mq_close(mq);
    sem_close(workers);
    sem_close(jobs);
}

//Designed to be run alone in main
void test_for_producer(){
    char* filename = "input.txt";
    int ret;

    qAttr.mq_curmsgs = 0;
    qAttr.mq_flags = 0;
    qAttr.mq_maxmsg = MAX_MSG;
    qAttr.mq_msgsize = MSG_SIZE;

    mq_unlink(MESS_QUEUE);
    sem_unlink(WORKERS_SEM);

    if(fork()) {
        producer(1, filename, 1);
        exit(0);
    }
    else {
        const int MSG_COUNT = 11; //matches expected line count of file.
        sem_t *workers;
        sem_t *jobs;
        mqd_t mq;
        char buff[MSG_SIZE];

        workers = sem_open(WORKERS_SEM, O_CREAT | O_RDWR, 0666, 0);
        jobs = sem_open(JOBS_SEM, O_CREAT | O_RDWR, 0666, 0);

        mq = mq_open(MESS_QUEUE, O_CREAT | O_RDONLY, 0644, &qAttr);
        CHECK(mq == (mqd_t)-1, "producer_test's mq_open");

        for (int i = 0; i < MSG_COUNT; i++) {
            ret = sem_post(workers);
            CHECK(ret == -1, "test_producer's sem_post");

            ret = sem_wait(jobs);
            CHECK(ret == -1, "test_producer's sem_wait");

            ret = mq_receive(mq, buff, sizeof(buff), 0);
            CHECK(ret == -1, "test_producer's mq_receive");

        }
        
        sem_close(workers);
        sem_close(jobs);
        mq_close(mq);
    }

    wait(NULL);

    sem_unlink(WORKERS_SEM);
    sem_unlink(JOBS_SEM);
    mq_unlink(MESS_QUEUE); 
}

void test_for_consumer(){
    qAttr.mq_curmsgs = 0;
    qAttr.mq_flags = 0;
    qAttr.mq_maxmsg = MAX_MSG;
    qAttr.mq_msgsize = MSG_SIZE;

    mq_unlink(MESS_QUEUE);
    sem_unlink(WORKERS_SEM);

    char str[] = "Send a fax,1000"; //test message
    int res;

    //open sem
    sem_t *workers = sem_open(WORKERS_SEM, O_CREAT | O_RDWR, 0666, 0);
    CHECK(workers == SEM_FAILED, "sem_open durring test");
    //open sem
    sem_t *jobs = sem_open(JOBS_SEM, O_CREAT| O_RDWR, 0666, 0);
    //open queue
    mqd_t mq = mq_open(MESS_QUEUE, O_CREAT | O_WRONLY, 0644, &qAttr);
    CHECK(mq == -1, "mq_open durring test");

    //Call consumer to activet
    if (!fork()) {
        consumer(1);
        exit(0);
    }

    sem_wait(workers);
    res = mq_send(mq, str, sizeof(str), 0);
    sem_post(jobs);
    sem_wait(workers);
    res = mq_send(mq, str, sizeof(str), 0);
    sem_post(jobs);
    sem_wait(workers);
    res = mq_send(mq, EXIT, sizeof(EXIT), 0);
    sem_post(jobs);

    wait(NULL);
    CHECK(sem_close(workers) == -1, "closing");
    CHECK(sem_unlink(WORKERS_SEM) == -1, "closing");
    CHECK(sem_close(jobs) == -1, "closing");
    CHECK(sem_unlink(JOBS_SEM) == -1, "closing");
    CHECK(mq_close(mq) == -1, "closing");
    CHECK(mq_unlink(MESS_QUEUE) == -1, "closing");
    return;
}

void main2(){
    // test_for_consumer();
    test_for_producer();
}

void main(int argc, char** argv) {

    // Over write arguments //////////////////
    argc = 3;
    char *test_args[] = {"Run","input.txt","4"};
    argv = test_args;
    //////////////////////////////////////////

    struct stat st;

    qAttr.mq_curmsgs = 0;
    qAttr.mq_flags = 0;
    qAttr.mq_maxmsg = MAX_MSG;
    qAttr.mq_msgsize = MSG_SIZE;

    //Error check arguments
    bool valid = true;
    if (argc != 3) {
        printf("Invalide arguments\n");
        printf("Example: './a.out <filename> <thread_count>\n\n");
        exit(0);
    }
    char* filename = argv[1];
    int consumer_count = atoi(argv[2]);

    //Check if file exists
    if (stat(argv[1], &st) == -1) {
        printf("Error Argument 1: %s. The file does not exist!\n", filename);
        valid = false;
    }
    //Check consumer_count (argv[2]) validity
    if (consumer_count < 0 || consumer_count > 98) {
        printf("Argument 2 must be a number between 0 and 99!\n");
        valid = false;
    }
    if (!valid)
        exit(0);


    //Async run consumer processes
    for (int i = 0; i < consumer_count; i++) {
        if (!fork()) {
                consumer(i);
                exit(0);
        }
    }

    //Run the producer process
    producer(1, filename, consumer_count);

    //Unlink the named queue and sems
    mq_unlink(MESS_QUEUE);
    sem_unlink(WORKERS_SEM);
    sem_unlink(JOBS_SEM);

    return;
}