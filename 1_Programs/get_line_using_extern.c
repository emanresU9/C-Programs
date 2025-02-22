#include <stdio.h>

#define MAX_LINE 1000
char line[MAX_LINE];
int get_line(void);

int main() {
    extern char line[];
    get_line();
    printf("%s", line);
}
int get_line(void) {
    extern char line[];
    int i = -1;
    int c;
    while(i < MAX_LINE-1 && (c = getchar()) != EOF && c != '\n') 
        line[++i] = c;
    if (c == '\n')
        line[++i] = c;
    line[++i] = '\0';

    return i;
}