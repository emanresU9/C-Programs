#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

struct stat st = {0};

int main(int argc, char **argv) {
    if (stat(argv[1],&st) == -1)
        mkdir(argv[1], 0700);
    return 0;
}