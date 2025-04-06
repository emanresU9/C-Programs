#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>


struct thread_args{
    int pos;
    int length;
};

void *worker(void *ptr) {
    struct thread_args args = *(struct thread_args*)ptr;
    printf("Pos: %d\nLength: %d\n", args.pos, args.length);
}

void main(){
    printf("\n\nStarting Program\n\n");
    pthread_t th[4];
    struct thread_args args[4];
    bool detected_error = false;
    char str[] = "My String\n";
    for (int i = 0; i < 4; i++) {
        args[i].pos = i;
        args[i].length = 58;
        pthread_create(&th[i], NULL, worker, (void*)(&args[i]));
    }
    for (int i = 0; i < 4; i++) {
        if ((pthread_join(th[i], NULL)) != 0) {
            printf("Error during %ld thread join\n", th[i]);
            detected_error = true;
        }
    }
    if (detected_error)
        exit(2);
}
