#include <stdio.h>

int get_a_line(char line[], int MAX_LINE) {
    int c, i = 0;
    while (i < MAX_LINE-1 && (c = getchar()) != '\n' && c != EOF) {
        line[i] = c;
        i++;
    }
    if (c == '\n') {
        line[i] = c;
        i++;
    }
    line[i] = '\0';

    return i;
}