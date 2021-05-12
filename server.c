#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include <netinet/in.h>
#include <string.h>

#include <pthread.h>
#include <errno.h>
#define CLIENT 5
#define WELCOME "=====Benvenuto/a nella chatroom=====\ninserisci il tuo nome: "

struct client{
    char name[256];
    int socket;
};


void* sendT(void* c){

    int client = *(int*)c;

    while(1){  

        char msg[256];
        printf("inserisci un messaggio\n");  
        scanf("%s",&msg);

        if(send(client,msg,sizeof(msg),0)==-1)
            perror("messaggio non inviato");
    }
}


void* receive(void* c){
    struct client client= *(struct client*)c;
    //int client = (struct client)c->socket;
    int f ;
     printf("[%s]: e' entrato nella chatroom!!\n",client.name);
        char server_response[256] ;
	    while(1){
            if(f =recv(client.socket,&server_response,sizeof(server_response),0)>0)
                 printf("[%s]: %s",client.name,server_response);
           // printf("%d %s",f,server_response);
            if(f==-1)
                perror("errore di ricezione");
            if(f==0)
              break;
           
                
                // fflush(stdout);
        }
      
       close(client.socket);
       printf("%s ha lasciato la stanza\n",client.name);
    

}


int main(int argc, char* argv[]){
    //creazione socket e gestione errore
    int server;
    server = socket(AF_INET,SOCK_STREAM,0);
    if(server==-1)
        perror("socket non creato\n");

    //Struttura del socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(10030);
    server_address.sin_addr.s_addr =INADDR_ANY;
    
    //bind socket con struttura e gestion errore
    if(bind(server,(struct sockaddr*) &server_address,sizeof(server_address))==-1)
        perror("binding non riuscito");
    
    //ascolto delle connessioni e gestione e rrore
    if(listen(server,CLIENT)==-1)
        perror("socket non in ascolto");
   

    //accettazione connessioni
    while(1){

        struct client client;
        struct sockaddr_in client_address;
        int len =sizeof(client_address);
     
        printf("waiting clients...\n");
	    client.socket = accept(server,(struct sockaddr *)&client_address,&len); 
        if(send(client.socket,WELCOME,sizeof(WELCOME),0)==-1)
            perror("messaggio non inviato");
        memset(&client.name, 0, sizeof(client.name));
        if(recv(client.socket,&client.name,sizeof(client.name),0)<0)
            perror("dati non ricevuti");
            
       
        pthread_t tid;
        pthread_create(&tid,NULL,receive,&client);
    }


}
