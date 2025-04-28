#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

// READ.ME //
// The proper syntax for executing this command is:
//     /a.out filename char int1 int2 int3
// where int1 is the number of threads to star and
// int2 and int3 are the nice setting for the 
// counter thread and cpu eater thread(s) 
// respectively

/////////// Function Declarations //////////////
int count_primes(int t);
char* fileReader(char *filename);
int parseInt(char * str);

///////////// Global Variables ////////////////
struct timeval start, stop;
char searchChar;
int charCounter;
double elapsed;
int prio1;
int prio2;

////////////// Thread runners ////////////////
void *pthread_counter(void* ptr) {
    //Cast the void* argument
    char* file_buf = (char*) ptr;
    //Set the priority of this thread
    setpriority(PRIO_PROCESS, gettid(), prio1);
    // // Display the this processes nice value
    // printf("nice value of counter: %d\n",getpriority(PRIO_PROCESS, gettid()));
    charCounter = 0;

    //loop for all chars in file_buf while counting characters matching searchChar
    for (int i = 0; file_buf[i] != '\0'; i++) {
        if (file_buf[i] == searchChar || file_buf[i] == (int)(searchChar - 31))
            charCounter++;
    }

    // End timer
    gettimeofday(&stop, NULL);
    elapsed = start.tv_usec <= stop.tv_usec ? 
        (double)(stop.tv_sec - start.tv_sec) + (double)(stop.tv_usec - start.tv_usec)/1000000 :
        (double)(stop.tv_sec - start.tv_sec -1) + (double)(1000000 + stop.tv_usec - start.tv_usec)/1000000; 

    return NULL;
}

void *pthread_waster(void* ptr) {
    setpriority(PRIO_PROCESS, gettid(), prio2);
    // printf("nice value of waster : %d\n",getpriority(PRIO_PROCESS, gettid()));
    int count = count_primes(100100);
    return NULL;
}

///////////////// MAIN /////////////////
int main(int argc, char **argv) {
    searchChar;
    charCounter = 0;
    int priority;
    int thread_num;
    char* filename;
    char* file_buf;
    
    filename = argv[1];
    searchChar = argv[2][0];
    thread_num = parseInt(argv[3]);
    prio1 = parseInt(argv[4]);
    prio2 = parseInt(argv[5]);
    file_buf = fileReader(filename);
    
    pthread_t th[thread_num + 1];

    printf("=====================================================\n");
    printf("|| Starting Program: Count '%c' chars in %s\n",searchChar, filename);
    printf("=====================================================\n");
    //Display priority levels
    printf("|| Creating %d threads with niceness of %d\n", thread_num, prio2);
    printf("|| (Counter thread has niceness of %d)\n", prio1);

    // Run high priority thread(s)
    for (int i = 0; i < thread_num; i++)
        pthread_create(&th[i], NULL, pthread_waster, NULL);

    // Start timer
    gettimeofday(&start, NULL);

    // Run low priority thread
    pthread_create(&th[thread_num], NULL, pthread_counter, (void*)file_buf);

    // Join low priority thread
    pthread_join(th[thread_num], NULL);

    // Join high priority thread(s)
    for (int i = 0; i < thread_num; i++) {
        pthread_join(th[i], NULL);
    }

    printf("||\n|| RESULTS\n");
    printf("||\tCharCount: %d\n", charCounter);
    printf("||\tTime Elapsed: %f\n", elapsed);
    printf("|| Done\n=====================================================\n");
    return 0;
}

//////////// Function Definitions ///////////////

// Function: parseInt
// input:    A String representing a number
// output:   An integer 
int parseInt(char * str) {
    int result = 0;
    int sign = 1;
    int i = 0;
    if (str[i] == '-') {
        sign = -1;
        i++;
    }
    else if(str[i] == '+')
        i++;
    for (; str[i] != '\0' && str[i] >= '0' && str[i] <= '9'; i++) {
        result = result*10 + str[i] - '0';
    }
    if (str[i] != '\0')
        printf("Error parsing integer");
    result *= sign;
    return result;
}

// Function: fileReader
// input:  filename is the name of the file.
// output: returns a pointer to the string containing
//  the file's contents.
char* fileReader(char *filename) {
    int fd;
    char * fileBuffer;
    int ret;
    struct stat s;

    stat(filename, &s);

    //Open the file
    if ((fd = open(filename, O_RDONLY)) == -1) {
        printf("Error on open\n");
        exit(2);
    }
    //Allocate the fileBuffer then read from the file
    fileBuffer = malloc(s.st_size+1);
    if ((ret = read(fd, fileBuffer, s.st_size)) == -1) {
        printf("Error reading from %s\n", filename);
        exit(3);
    }
    fileBuffer[s.st_size] = '\0';
    return fileBuffer;
} 

// function: count_primes
// input: t is the larges value for a prime.
// output: returns the prime_counter which 
//  has been incremented once for each prime
int count_primes(int t) {
    bool prime = true;
    int count = 0;
    for (int i = 1; i < 100000; i++) {
        prime = true;
        for (int j = 2; j <= i/2; j++) {
            if (i % j == 0) {
                prime = false;
                break;
            }
        }
        if (prime)
            count++;
    }
    return count;
}