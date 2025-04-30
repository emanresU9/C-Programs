#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition, location_msg) \
do { \
    if ((condition)) { \
        perror(location_msg); \
        fprintf(stderr, "ERROR: line %d\n", __LINE__); \
    } \
} while(0)

#define MSG_SIZE 100

void main(){
    int idx, res;
    char exeTime[11];
    int consumerID = 1;
    char str[] = "asdf,1000";
    //Find index of comma
    char* pos = str;
    while(*pos != ',' && pos != str+MSG_SIZE)
        pos++;
    if (pos == str+MSG_SIZE) {
        printf("Invalid Message: Consumer exiting");
        exit(1);
    } 
    else
        idx = (int)(pos - str);

    //extract the exeTime portion of str
    res = snprintf(exeTime, sizeof(exeTime), "%s", str+idx+1);
    CHECK(res < 0, "consumer's snprintf");
    
    //remove the exeTime portion from str
    str[idx] = '\0';

    //Display
    printf("consumer %d | %s | %d\n", consumerID, str, atoi(exeTime));
}