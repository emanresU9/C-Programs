#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

struct args_get_char_counts {
    char *buff;
    int start;
    int max;
    int* counter;  //points to an array of size 26
    int thread_count;
};

void printFile(char* file_buffer, int st_size);         
void displayTotals(int* char_counts, int chars_total);

void *get_char_counts(void* ptrv) {
    struct args_get_char_counts args = *(struct args_get_char_counts*) ptrv;
    int i = args.start;
    int* counter = args.counter;
    char* file_buffer = args.buff;
    int thread_count = args.thread_count;

    // // Check to see if buffer is full
    // putchar(args.start + '0');
    // putchar(' ');
    // for (int i = 0; i < args.max; i++) {
    //     putchar(file_buffer[i]);
    // }
    // putchar ('\n');

    for (int j = 0; j < 27; j++)
        counter[j] = 0;
    for (; i < args.max; i += thread_count) {
        if (file_buffer[i] - 'a' >= 0 && file_buffer[i] - 'a' < 26)
            counter[file_buffer[i] - 'a'] ++;
        else if (file_buffer[i] - 'A' >= 0 && file_buffer[i] - 'A' < 26)
            counter[file_buffer[i] - 'A'] ++;
        else 
            counter[26] ++;
    }
    return NULL;
}

int main(int argc, char** argv) {
    int thread_count = (int)(argv[1][0] - '0');
    struct timeval t1, t2;

    pthread_t thread[thread_count];
    struct args_get_char_counts argt[thread_count];

    struct stat s;      //s will contain file information
    stat(argv[2], &s);  

    char* file_buffer;  //file_buffer holds chars
    file_buffer = malloc(s.st_size);

    //Open the file
    int fd;
    if ((fd = open(argv[2], O_RDONLY)) == -1){
        printf("Error opening file\n");
        exit(1);
    }
    //Get the file contents into file_buffer
    if (read(fd, file_buffer, s.st_size) != s.st_size) {
        printf("Error reading file\n");
        close(fd);
        exit(2);
    }
    close(fd);

    // // Display file txt
    // printf("File Contents %s file\n\t", argv[1]);
    // printFile(file_buffer, s.st_size);

    printf("Starting timer:\n");
    gettimeofday(&t1, NULL);

    for (int i = 0; i < thread_count; i++) {
        argt[i].buff = file_buffer;
        argt[i].start = i;
        argt[i].max = s.st_size;
        argt[i].counter = (int *)malloc(27*sizeof(int));
        argt[i].thread_count = thread_count;
        pthread_create(&thread[i], NULL, get_char_counts, (void *) &argt[i]);
    }
    for (int i = 0; i < thread_count; i++) {
        pthread_join(thread[i], NULL);
    }

    gettimeofday(&t2, NULL);
    printf("Timer ended\n");

    double elapsed_time;
    if (t2.tv_usec >= t1.tv_usec)
        elapsed_time = (double)(t2.tv_sec - t1.tv_sec) + ((double)(t2.tv_usec - t1.tv_usec))/1000000;
    else 
        elapsed_time = (double)(t2.tv_sec - t1.tv_sec) -1.0 + (1000000.0 + (double)t2.tv_usec - (double)t1.tv_usec)/1000000.0;

    printf("elapsed_time = %f\n", elapsed_time);

    printf("\nDisplaying Character Counts\n");

    int totals[27];
    for (int i = 0; i < 27; i++) { totals[i] = 0;}
    for (int i = 0; i < thread_count; i++) {
        for (int j = 0; j < 27; j++) {
            totals[j] += argt[i].counter[j];
        }
    }
    int total = 0;
    for (int i = 0; i < 26; i++)
        total += totals[i];


    displayTotals(totals, total);

    for (int i = 0; i < thread_count; i++) {
        free(argt[i].counter);
    }
    return 0;
}
void printFile(char* file_buffer, int st_size) {
    for (int i = 0; i < st_size; i++) {
        putchar(file_buffer[i]);
    }
    putchar('\n');
}
void displayTotals(int* char_counts, int chars_total) {
    printf("{a-z} -> ");
    printf("{\n");
    for (int i = 0; i < 26; i++) {
        printf("\t%d ", char_counts[i]);
        if ((i+1) % 5 == 0) 
            putchar('\n');
    }
    printf("\n}\n");
    // printf("\nOther: %d\n", char_counts[26]);
    printf("Total: %d\n", chars_total);
}