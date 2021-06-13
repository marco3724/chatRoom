#ifndef STRUCT_QUEUE
#include <pthread.h>
#include "defaultServer.h"
#include "structClient.h"
struct coda{
    int start;
    int end;
    struct message buffer[QUEUE_SIZE];
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
};


void initQueue(struct coda* );
void storeMessage(struct coda *queue,char* msg,char* time, struct client* client,char* color);
struct message* getMessage(struct coda *);
#endif
