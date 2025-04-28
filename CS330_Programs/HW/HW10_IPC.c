#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <semaphore.h>
#include <fcntl.h>
#include <printf.h>
#include <mqueue.h>

char** gen_process_names(int many) ;

struct semaphore {
    sem_t s;
    msqueue
};

int main(int argc, char** argv)
{
    const int P_COUNT = 10;
    char **p_names = gen_process_names(P_COUNT);

    sem_t* s = sem_open("mySem", O_CREAT, 0600, 1);

    mqd_t jobQ;
    struct mq_attr mqAttr;
    mqAttr.mq_flags = 0;
    mqAttr.mq_maxmsg = 10;
    mqAttr.mq_msgsize = 20;
    mqAttr.mq_curmsgs = 0;


    free(p_names);
    return 0;
}


char** gen_process_names(int many) {
    char **p_names = malloc(many * sizeof(char*));
    char *prepend = "proc_";
    for (int i = 0; i < many; i++) {
        p_names[i] = malloc(5+5);
        sprintf(p_names[i], "%s%d", prepend, i);
    }
    return p_names;
}
void run_process(int pID) {
    printf("Running $d\n",pID);
}