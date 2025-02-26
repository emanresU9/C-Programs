#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
// #include "Timeit.c"
#include <sys/time.h>

int cols = 3;
int rows = 12;
int cCount = 26;
char c;
pid_t PID, pid;

struct timeval get_my_timeval();
double compare_timevals(struct timeval end, struct timeval start);

int main() {

    printf("Starting program\n\n");
    printf("starting timer\n\n");

    struct timeval start = get_my_timeval();

    PID = getpid();
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            for (int k = 0; k < 26; ++k) {
                if (fork() == 0) {
                    c = (char) (k + 65);
                    putchar(c);
                    exit(0);
                }
            }
            while(wait(NULL) != -1)
                ;
        }
        putchar('\n');
    }


    struct timeval end = get_my_timeval();
    double elapsed = compare_timevals(end, start);
    printf("\n\nElapsed time: %f\n", elapsed);


    return 0;
}
