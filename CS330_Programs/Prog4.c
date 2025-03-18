#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <stdbool.h>
#include <wait.h>
#include <string.h>
#include <math.h>


// This program is to show the timing 
// of forked processes when scheduled
// to use multiple CPUs vs when scheduled
// to only use one

//// PSEUDO CODE ////
// 1. BEGIN
// 2. FOR 4 iterations
// 3.   Fork() 
// 4.   IF in child process
// 5.     IF using multiple cores
//          set proccessor affinity to run on all CPUs
// 6.     ELSE
//          set processor affinity to use only one CPU
//        ENDIF using multiple cores
// 7.     Run a test program
// 8.     exit process
//      ENDIF in child process
//    ENDFOR 4 iterations
// 9. Wait for child processes to complete
// 10. exit process
// 11. DONE

#define MULTITHREADING false
#define THREADS 32
#define VERBOSE false

void test_program(int setNum);
void quickSortStrings(char *sArray[], int left, int right);


int main() {
    int cpu_count;
    cpu_set_t cSet;
    CPU_ZERO(&cSet);

    //Set cpu_count to the number of available processors.
    if (MULTITHREADING)
        cpu_count = sysconf(_SC_NPROCESSORS_ONLN);
    else 
        cpu_count = 1;

    //For loop fork then set processor affinity 
    for (int thread_i = 0; thread_i < THREADS; ++thread_i)
        if (fork() == 0) {
            //Set the CPU on the availble processors
            for (int i = 0; i < cpu_count; i++)
                CPU_SET(i, &cSet);
            sched_setaffinity(0, sizeof(cpu_set_t), &cSet);
            test_program(thread_i);
            exit(0);
        }
    
    // Wait for processes to finish and use status to hold
    // Exit code.
    int status;
    for (int i = 0; i < THREADS; i++) {
        wait(&status);
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == EXIT_FAILURE) {
                printf("Error occured in test_program\n");
                printf("Exit code: %d\n", (WEXITSTATUS(status)));
                exit(2);
            }
        }
    }
    return 0;
}

//displayStrArray() is used by test_program()
void displayStrArray(char *sArray[], int size, int setNum);

void test_program(int setNum){
    char *names[] = {"Zebra","Goose","Turtle","Penguin","Hippo","Frog","Leapard","Deer","Ape","Seagull","Bee", "Cow", "Horse", "Pig", "Chicken", "Bat", "Parot", "Mantis", "Dove", "Elephant", "Mantis", "Sheep", "Wolf", "Peggasis", "Conda", "Firefly", "Beetle", "Worm", "Spider", "Boxelder", "Ant", "Linx", "Cattapillar", "Ladybug"};
    int size = sizeof(names)/sizeof(names[0]);

    int cpu_portion = size/sysconf(_SC_NPROCESSORS_ONLN);

    int start = setNum*cpu_portion%size, end;
    if ((end = start + cpu_portion) >= size)
        end = size-1;
    quickSortStrings(names, start, end);

    if (VERBOSE) 
        displayStrArray(names, size, setNum);
}
void displayStrArray(char *sArray[], int size, int setNum) {
    for (int i = 0; i < size; i++) 
        printf("set #%d %s\n", setNum, sArray[i]);
}

//swap() and comp() are used by quickSortStrings()
int swap(char *sArray[], int i, int j);    
int comp(char *sArray[], int i, int j);

void quickSortStrings(char *sArray[], int left, int right) {
    if (left < right) {
        int min = left, max = right;
        swap (sArray, left, (left+right)/2);

        while (min < max) {
            while (min < max && comp(sArray,min,left) <= 0) {
                min++;
            }
            while (comp(sArray, max, left) > 0) {
                max--;
            }
        
            if (min < max) {
                swap(sArray, min, max);
            }
        }
        swap(sArray, left, max);
        quickSortStrings(sArray, left, right-1);
        quickSortStrings(sArray, max+1, right);
    }
}
int swap(char *sArray[], int i, int j) {
    char *temp = sArray[i];
    sArray[i] = sArray[j];
    sArray[j] = temp;
}
int comp(char *sArray[], int i, int j) {
    int result;
    int c_i = 0;
    while (sArray[i][c_i] == sArray[j][c_i]) {
        c_i++;
        if (sArray[i][c_i] == '\0')
            return 0;
    }
    result = (int)(sArray[i][c_i]) - (int)(sArray[j][c_i]);
    return result;
}