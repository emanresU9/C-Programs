#include <stdio.h>

int strin_dex(char src[], char target[]){
    int i, j, k;
    for (int i = 0; src[i] != '\0'; i++) {
        for (j = i, k = 0; target[k] != '\0' && src[j] == target[k]; j++, k++)
            ;
        if (k > 0 && target[k] == '\0')
            return i;
    }
    return -1;
}