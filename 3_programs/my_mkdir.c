#include <stdio.h>

int main(int argc, char **argv) {
    printf("You havee  entered %d arguments\n", argc);
    printf("Those args are:\n");
    for (int i = 0; i < argc; i++) {
        printf("%d\t%c\n", i, (argv[i])[0]);
    }
    return 0;
}