#include <stdio.h>


#define max 10000
#define QUIET false

// finds primes in range less than max
int main() {
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

return 0;
}