#include <stdlib.h>
#include <stdio.h>

int parseInt(char *str);

int main(int argc, char **argv) {
    printf("parsed Int: %d\n",parseInt(argv[1]));
    exit(0);
}
int parseInt(char * str) {
    printf("Start\n");
    int i = 0;
    int result = 0;
    // while (i != '\0') {
    //     result = result*10 + str[i] - '0';
    //     i++;
    // }
    while(str[i] != '\0') {
        printf("%c", str[i]);
        i++;
    }
    return result;
}