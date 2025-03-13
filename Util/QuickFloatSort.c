#include <stdio.h>
#include <stdlib.h>
#include "StringConversions.c"

void sort(float[], int,int);

int main(int argc, char **argv) {

    float arry[argc-1];
    int count = 0;
    for (int i = 1; i < argc; i++){
        arry[i-1] = s_to_f(argv[i]);
        count++;
    }

    sort(arry,0,count-1);

    for (int i = 0; i < count; i++) {
        printf("%.3f\n",arry[i]);
    }

    exit(0);
}
void swap(float[],int,int);
void sort(float arry[], int left, int right){
    int mid = left;
    int min = left, max = right;
    if (left < right) {
        swap(arry,mid,(left + right)/2);
        while (left < right) {
            while (left < right && arry[left] <= arry[mid])
                left++;
            while (arry[right] > arry[mid])
                right--;
            if (left < right)
                swap(arry,left,right);
        }
        swap(arry,mid,right);
        sort(arry,min, right-1);
        sort(arry,right+1,max);
    }
}
void swap(float arry[], int i, int j) {
    float temp;
temp = arry[i];
arry[i] = arry[j];
arry[j] = temp;
}

