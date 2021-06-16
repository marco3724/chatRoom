
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include "./headers/structClient.h"
//radice della catena dei client
struct client root = {"root",-1,NULL,NULL,NULL};

void sendtoAll( struct client *client,void *msg){
    char * message = (char*)msg;
    struct client *node = root.next;//setto come nodo il primo lcient della catena

    fprintf(stdout,"\r%s",message);//stampo sul server
    
    //formatto
    printf("\r> ");
    fflush(stdout);

    //manda il messaggio a tutti i nodi
    while(node->next !=NULL){
        if(send(node->socket,message ,FULL_MEXSIZE,0)==-1)//invia il messaggio
    		perror("messaggio non inviato");
        node = node->next;//scorro la catena
    }
}

void addNode(struct client *client,struct client** node){
      
    (*node)->next = client;//aggiungo il client all'ultimo nodo della catena
    client->prev = (*node);//come client precedente metto il nodo
    *node = client;//setto l'ultimo nodo della catena dei client = client
    client->next = NULL;//il prossimo client di client e'  NULL
  
}

//per come ho strutturato la creazione del client,
// l'ultimo client della catena non puo' essere NULL ma sara sempre un client che non si e; acnora connesso
void removeNode(struct client *client){

    if(client->prev==&root ){//vuol dire che e il primo, lego la root con l'ultimo client che non e' ancora connesso
        client->next->prev =&root;//il client ancora non connesso si lega a root
        root.next = client->next;//il primo client diventa il client ancora non connesso
    }
    else{//vuol dire che sta in mezzo, elimino il client legando il suo successivo con il suo precendente e vicevera
        client->prev->next = client->next;
        client->next->prev=client->prev;
    }
}
