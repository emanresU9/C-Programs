#include <stdio.h>
#include <limits.h>

long power(int base, int exp);

int main() {
    int char_MAX = power(2,8);
    int integer_MAX = power(2,31);
    printf("Max value for an object of type int: %d\n", INT_MAX);
    printf("Min value for an object of type int: %d\n", INT_MIN);
    printf("Check: %d\n", (int) power(2,31)-1);
    printf("\nMax value for an object of type long: %ld\n", LONG_MAX);
    printf("Min value for an object of type long: %ld\n", LONG_MIN);
    printf("Check: %ld\n", power(2,63) -1);
    return 0;
}
long power(int base, int exp) {
    long p;
    for (p = 1; exp > 0; exp--) {
        p = p * base;
    }
    return p;
}