#ifndef STRUCT_QUEUE_H
#define STRUCT_QUEUE_H
#include <pthread.h>
#include "defaultServer.h"
#include "structClient.h"
#include <stdlib.h>
/*
struttura che serve ad immagazzinare messaggi
start: indica l'inizio della coda
end:indica la fine della coda
mutex: serve ad impedire a piu processi di utilizzarla contemporaneamente
not_empty: indica che la coda non e' vuota,in questa condizione si puo' utilizzare la coda in lettura
not_full indica che la coda non e' piena, in questa condizione si puo' utilizzare la coda in scrittura
*/
struct coda{
    int start;
    int end;
    int real_size;
    struct message buffer[QUEUE_SIZE];
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
};

//inizializza la coda
void initQueue(struct coda* );

//immagazzina un messaggio nella coda
void storeMessage(struct coda *queue,char* msg,char* time, struct client* client,char* color);

//restituisce il primo messaggio dalla coda
struct message* getMessage(struct coda *);

//riordina la coda
void sort(struct coda *queue);
#endif
