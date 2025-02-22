#include <stdio.h>
#include <limits.h>
#include <float.h>

long power(int base, int exp);

int main() {
    int i1 = CHAR_BIT;
    int i2 = CHAR_MAX;
    int i3 = CHAR_MIN;
    int i4 = INT_MAX;
    int i5 = INT_MIN;
    long l1 = LONG_MAX;
    long l2 = LONG_MIN;
    short s1 = SHRT_MAX;
    short s2 = SHRT_MIN;
    int i6 = UCHAR_MAX;
    __u_int i7 = UINT_MAX;
    __u_long l3 = ULONG_MAX;
    __u_short s3 = USHRT_MAX;

    printf("CHAR_BIT %d\nCHAR_MAX: %d\nCHAR_MIN: %d\nINT_MAX: %d\nINT_MIN: %d\nUCHAR_MAX: %d\nUINT_MAX: %u\nULONG_MAX: %lu\nUSHRT_MAX: %d\n",i1,i2,i3,i4,i5,i6,i7,l3,s3);

    return 0;
}
long power(int base, int exp) {
    long p;
    for (p = 1; exp > 0; exp--) {
        p = p * base;
    }
    return p;
}