// name: Chris Wang
// course: CS330
// date: 2.8.2025
// Homework Label: HW2_Code_Timer
// Description: The purpose of this
//   program is to demonstrate the 
//   use of gettimeofday. 

// Code_Timer Pseudo-code
// 1. Get start time
// 2. Run test program
// 3. Get end time
// 4. Get diff
// 5. Display diff
// END

#include <stdio.h>
#include <sys/time.h>
#include <stdbool.h>
#include <unistd.h>

#define TEST_VALUE 1000000 //Used by test function
#define QUIET true //To quiet the output of the test function


    // fuction delcarations /////////////////
    void test_function_find_primes(int range);
    struct timeval get_time_val();
    double difftime(struct timeval end, struct timeval start);


int main() {
    struct timeval start, end;
    double diff_time;

    // print msg
    printf("\nRunning tests...\n");

    //get start time
    start = get_time_val();

    //run test
    test_function_find_primes(TEST_VALUE);

    //get end time
    end = get_time_val();

    //print msg
    printf("Done\n");

    //get elapsed_time
    diff_time = difftime(end, start);

    //Display
    printf("\n");
    printf("elapsed time in seconds: %f \n\n", diff_time);

    return 0;
}

// finds primes in range less than max
void test_function_find_primes(int max) {
    // isPrime stays true until disproven in a loop
    bool isPrime = true;
    // count is for display
    int count = 0;
    // FOR loop integers less than max
    for (int i = 1; i <= max; i++){
        // FOR loop divides i by each j
        for (int j = 2; j <= i/2; j++) {
            // IF remainder is 0
            if (i%j == 0) {
                // i is not prime 
                isPrime = false;
                // break;
                break;
            }
        }
        // IF isPrime stayed true
        if (isPrime) {
            // increment count
            // and add newline 
            // every 10 primes
            if (!QUIET && ++count %10 == 0) {
                printf("\n");
            }
            // display prime
            if (!QUIET)
                printf("%d ",i);
        }
        // reset isPrime to true
        isPrime = true;
    }
    if (!QUIET)
    printf("\n");
}

//set a time val and return it
struct timeval get_time_val(){
    struct timeval sys_time;
    gettimeofday(&sys_time, NULL);
    return sys_time;
}

//find the difference between end and start and return the result
double difftime(struct timeval end, struct timeval start) {
    double result;
    result = (end.tv_sec - start.tv_sec);
    result += (end.tv_usec - start.tv_usec) / (double) 1000000;
    return result;
}

