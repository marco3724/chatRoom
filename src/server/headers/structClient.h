#ifndef STRUCTCLIENT_H
#define STRUCTCLIENT_H
#include "defaultServer.h"


//struttura del server per gestire i client


struct client{ 
    char name[NAME_SIZE]; //nome client
    int socket; //socket su cui inviare i messaggi
    FILE *log; //file di log singolo del client
    struct client *prev; //client precedente
    struct client *next ; //client successivo
};
struct client root; //radice della catena dei cleint

//invia un messaggio a tutti i client della catena
void sendtoAll( struct client *,void *);

//aggiunge un nodo alla catena
void addNode(struct client *,struct client** );

//rimuove un client
void removeNode(struct client *);
#endif