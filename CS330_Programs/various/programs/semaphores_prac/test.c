#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

// #define CHECK(cond, str) \
// if ((cond) == 1) { \
//     fprintf(stderr, "ERROR: %s: ", str); \
//     perror(""); \
//     exit(EXIT_FAILURE); \
// }

#define CHECK(cond, msg) \
    do {\
        if (cond) {\
            fprintf(stderr, "Error: %s (in file: %s at line %d)\n", msg, __FILE__, __LINE__);\
            exit(EXIT_FAILURE);\
        }\
    } while (0)

int main() {
    CHECK(1, "process1");
}