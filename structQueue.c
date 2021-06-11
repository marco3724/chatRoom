#include "structQueue.h"
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>



void initQueue(struct coda* queue){
    if (pthread_mutex_init(&queue->mutex, NULL) != 0) {
        fprintf(stderr, "Error in pthread_mutex_init()\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_cond_init(&queue->not_empty, NULL) != 0) {
        fprintf(stderr, "Error in pthread_cond_init()\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_cond_init(&queue->not_full, NULL) != 0) {
        fprintf(stderr, "Error in pthread_cond_init()\n");
        exit(EXIT_FAILURE);
    }
    for(int i =0;i<QUEUE_SIZE;i++)
        memset(queue->buffer[i],'\0',MES_SIZE);
    //char *c = "ciao";
    queue->start = queue->end = 0;
}

void storeMessage(struct coda *queue,char* msg){
    int rEnd; //real end
    if (pthread_mutex_lock(&queue->mutex) != 0) {//blocco
            perror("pthread_mutex_lock");
            exit(EXIT_FAILURE);
        }
    rEnd = (queue->end + 1) % QUEUE_SIZE; //calcolo la fine del buffer 
        
    while (rEnd == queue->start) { //se aggiungendo un elemento, la fine e' uguale all'inizio allora e' pieno
            
        if (pthread_cond_wait(&queue->not_full, &queue->mutex) != 0) {//sblocco il mutex per dare la possibilita' di consumare, aspetto che qualsuno me lo segnali e rimetto il lock
            perror("pthread_cond_wait");
            exit(EXIT_FAILURE);
        }
        rEnd = (queue->end + 1) % QUEUE_SIZE;//ricalcolo la fine
    }

    strncpy(queue->buffer[queue->end],msg,MES_SIZE ); //se non e' pieno aggiungo il messaggio alla fine
    queue->end = rEnd; //aggiorno il puintatnore che punta alla fine

    if (pthread_cond_signal(&queue->not_empty) != 0) {//dopo aver inserito, segnalo che non e' piu vuoto
        perror("pthread_cond_signal");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_unlock(&queue->mutex) != 0) {//sblocco il mutex
        perror("pthread_mutex_unlock");
        exit(EXIT_FAILURE);
    }
}
    





char* getMessage(struct coda *queue) {
    if (pthread_mutex_lock(&queue->mutex) != 0) {//blocco il mutex
        perror("pthread_mutex_lock");
        exit(EXIT_FAILURE);
    }
    while (queue->start == queue->end) { // vuol dire che e' vuoto
        if (pthread_cond_wait(&queue->not_empty, &queue->mutex) != 0) { //sblocco il mutex per permettere a qualcuno di riempirlo, aspetto finche non ricevo un segnale che mi dice che non e' piu vuoto e riblocco il mutex
            perror("pthread_cond_wait");
            exit(EXIT_FAILURE);
        }
    }

    char* msg = queue->buffer[queue->start]; //consumo il messaggio
    queue->start = (queue->start+1)%QUEUE_SIZE; // e incremento il contantore(rendendo invalido questo dato)

    if (pthread_cond_signal(&queue->not_full) != 0) { //segnalo che il buffer non e' piu pieno in quanto ho appena consumato
        perror("pthread_cond_signal");
        exit(EXIT_FAILURE);
    }
    if (pthread_mutex_unlock(&queue->mutex) != 0) { //sblocco il mutex
        perror("pthread_mutex_unlock");
        exit(EXIT_FAILURE);
    }
    return msg;

}
void sort(struct coda *queue){

}
/*
int main(){
    struct coda *q;
    initQueue(q);
    for(int i = 0;i<109;i++)
        storeMessage(q,"ciao");
    for(int i = 0;i<20;i++)
        printf("%s\n",getMessage(q));
    
}*/
