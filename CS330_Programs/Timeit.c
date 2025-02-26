#include <sys/time.h>
#include <stdlib.h>


struct timeval get_my_timeval(){
    struct timeval current;
    gettimeofday(&current,NULL);
    return current;
}
double compare_timevals(struct timeval end, struct timeval start) {
    double result;
    result = (end.tv_sec - start.tv_sec);
    result += (end.tv_usec - start.tv_usec) / (double) 1000000;
    return result;
}