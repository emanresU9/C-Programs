#include <stdio.h>
#include <stdlib.h>

#define NUMBER 0

int getop(char []);
double pop(void);
void push(double);

int main() {
    int type; 
    const int MAX_LEN = 100;
    char s[MAX_LEN];

    while ((type = getop(s)) != EOF) {
        switch (type) {
            case NUMBER:
                push(atof(s));
                break;
            case '+':
                
                break;
        }
    }
    return 0;
}