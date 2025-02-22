#include <stdio.h>

int func();

int line[5];

int main() {
    extern int line[];
    for (int i = 0; i < 5; i++){
        line[i] = i;
    }

    func();

    return 0;
}

int func() {
    // extern int line[];

    for (int i = 0; i < 5; i++) {
        printf("\n%d",line[i]);
    }
    printf("\n");

    return 0;
}