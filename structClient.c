#include "structClient.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
//radice della catena dei client
struct client root = {"root",-1,NULL,NULL,NULL};

void sendtoAll( struct client *client,void *msg){
    char * message = (char*)msg;
    struct client *node = root.next;

    //manda il messaggio a tutti i nodi
    while(node->next !=NULL){
        if(client ==node){
             node = node->next;
             continue; // se node_next e' null ci sarebbe errore, quindi faccio conitnue per rifare il cpontrolllo
        }
        if(send(node->socket,message ,256,0)==-1)
    		perror("messaggio non inviato");
        
        node = node->next;
    }
}

void addNode(struct client *client,struct client** node){
    // printf("CLIENTE E NODE 222  %p %p\n",client,node);    
    (*node)->next = client;
    client->prev = (*node);
    *node = client;
    client->next = NULL;
  //  printf("fatto NODE FUN %p\n",node);  
}

//per come ho strutturato la creazione del client,
// l'ultimo client della catena non puo' essere NULL ma sara sempre un client che non si e; acnora connesso
void removeNode(struct client *client){
    if(client->prev==&root ){//vuol dire che e il primo
        client->next->prev =&root;
        root.next = client->next;
    }
    else{//vuool dire che sta in mezzo
        client->prev->next = client->next;
        client->next->prev=client->prev;
    }
}
