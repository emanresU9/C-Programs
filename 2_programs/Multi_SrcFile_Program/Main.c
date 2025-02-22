#include <stdio.h>
#include "strin_dex.h"
#include "get_a_line.h"

int get_a_line(char[], int);
int strin_dex(char[], char[]);

int main() {
    char search[] = "ould"; 
    char line[1000];
    while (get_a_line(line, 1000) != 0) {
        if (strin_dex(line, search) != -1)
            printf("%s", line);
    }
    return 0;
}