#include <stdlib.h>

//Convert a String to a Float;
float s_to_f(char str[]){
    // printf("converting: %s\t", str);
    int i = 0, sign = 1, offset = 1;
    float result = 0;
    while (str[i] != '\0' && str[i] == ' ')
        i++;
    if (str[i] == '-') {
        sign = -1;
        i++;
    } else if (str[i] == '+'){
        sign = 1;
        i++;
    } 
    while (str[i] != '\0' && str[i] >= '0' && str[i] <= '9') {
        result *= 10;
        result += ((int)str[i] - (int)'0')%10;
        i++;
    }
    if (str[i] != '\0' && (str[i] < '0' || str[i] > '9') && str[i] != '.')
        exit(1);
    if (str[i] != '\0' && str[i] == '.') {
        i++;
        while (str[i] != '\0' && str[i] >= '0' && str[i] <= '9') {
            result *= 10;
            offset *= 10;
            result += ((int)str[i] - (int)'0')%10;
            i++;
        }
        if (str[i] != '\0' && (str[i] < '0' || str[i] > '9') && str[i] != '.')
        exit(2);
    }
    result = result * sign / offset;

    // printf("Added: %.2f\n",result);
    return result;
}