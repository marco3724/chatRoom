#ifndef STRUCTCLIENT_H
#define STRUCTCLIENT_H
#include "defaultServer.h"


/*struttura di un client
contiene il suo nome e il fd del socket
*/
struct client{ 
    char name[NAME_SIZE];
    int socket;
    FILE *log;
    struct client *prev;
    struct client *next ;
};
struct client root;


void sendtoAll( struct client *,void *);
void addNode(struct client *,struct client** );
void removeNode(struct client *);
#endif