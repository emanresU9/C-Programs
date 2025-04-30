#include <stdio.h>

void shellsort(int v[], int n, int g);
void printresult(int v[], int n);

int main() {
    int n = 9;
    
    int v[] = {5,1,7,4,3,7,5,1,0};
    printresult(v, 9);
    while ((n/=2) > 0) {
        shellsort(v, 9, n);
        printresult(v, 9);
    }
    return 1;
}
void printresult(int v[], int n) {
    printf("iter: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", v[i]);
    }
    putchar('\n');
}

void shellsort(int v[], int n, int gap) {
    int i, j, temp;

        for (i = gap; i < n; i ++) {
            for (j = i-gap; j >=0 && v[j] > v[j+gap]; j-= gap) {
                temp = v[j];
                v[j] = v[j+gap];
                v[j+gap] = temp;
            }
        }
}
