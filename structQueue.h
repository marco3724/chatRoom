#ifndef STRUCT_QUEUE
#include <pthread.h>
#include "defaultServer.h"
struct coda{
    int start;
    int end;
    char buffer[QUEUE_SIZE][MES_SIZE];
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
};


void initQueue(struct coda* );
void storeMessage(struct coda *,char*);
char* getMessage(struct coda *);
#endif
