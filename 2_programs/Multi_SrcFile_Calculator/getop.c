#include <stdio.h>
#include <ctype.h>
#include "calc.h"

/* Get the input from stdio of a 
single op. It could be an operand
or and operator, in any case you
must add it to s and return 0 if it
was a number or c if it was an operand
*/
int getop(char s[]) {
    int i, c;
    while ((s[0] = c = getch()) == ' ' | c == '\t')
        ;
    s[1] = '\0';
    if (!isdigit(c) && c != '.') {
        return c;
    }
    i = 0;
    if (isdigit(c)){
        while (isdigit(s[++i] = c = getch()))
            ;
    }
    if (c == '.')
        while (isdigit(s[++i] = c = getch()))
            ;
    s[i] = '\0';
    if (c != EOF)
        ungetch(c);
    return NUMBER;
}