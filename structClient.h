#ifndef STRUCTCLIENT_H
#include "default.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
/*struttura di un client
contiene il suo nome e il fd del socket
*/
struct client{ //TRASFROMARE IN TYPEDEF FORSE
    char name[NAME_SIZE];
    int socket;
    struct client *prev;
    struct client *next ;
};
struct client root;


void sendtoAll( struct client *,void *);
void addNode(struct client *,struct client** );
void removeNode(struct client *);
#endif