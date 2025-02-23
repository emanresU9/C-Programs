#include <stdio.h>
#include "calc.h"

#define LIMIT 100

double var[LIMIT];
int sp = 0;

void push(double d) {
    if (sp == LIMIT)
        printf("error: stack full, can't push %g\n", d);
    else 
        var[sp++] = d;  
}
double pop() {
    if (sp > 0)
        return var[--sp];
    else {
        printf ("error: stack empty\n");
        return 0.0;
    }
}