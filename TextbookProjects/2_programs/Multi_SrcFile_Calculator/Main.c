#include <stdio.h>
#include <stdlib.h>
#include "calc.h"

int main() {
    int type, sp;
    double pd1;
    const int MAX_LEN = 100;
    char s[MAX_LEN];
    char t;

    while ((type = getop(s)) != EOF) {
        switch (type) {
            case NUMBER:
                while((t = s[sp++]) != '\0')
                    ;
                push(atof(s));
                break;
            case '+':
                push(pop() + pop());
                break;
            case '*':
                push(pop() * pop());
                break;
            case '/':
                pd1 = pop();
                if (pd1 == 0)
                    printf("error: zero divisor\n");
                else 
                    push(pop() / pd1);
                break;
            case '\n':
                printf("\t%.8g\n", pop());
                break;
            default:
                printf("error: unknown command %s\n", s);
                break;
        }
    }
    return 0;
}