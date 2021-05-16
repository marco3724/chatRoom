#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <netinet/in.h>
#include <string.h>

#include <pthread.h>
#include <errno.h>
#include <time.h>

#define CLIENT 5
#define WELCOME "=====Benvenuto/a nella chatroom=====\ninserisci il tuo nome: "


/*struttura di un client
contiene il suo nome e il fd del socket
*/
struct client{
    char name[256];
    int socket;
    struct client *prev;
    struct client *next ;
};

struct client root = {"root",-1,NULL,NULL};

pthread_mutex_t mutexLog; //lock per accedere al file di log
FILE *fdLog; //file di log dei messaggi


void sendtoAll(){
    struct client *node = &root;
  while(node !=NULL){
        printf("client:%p    %p    %p    %d    %s\n",node->prev,node,node->next,node->socket,node->name);
        node = node->next;
    }
}


//ricezione di messaggi dai client
void* receive(void* c){
    struct client *client= (struct client*)c;
    
    int flag ;   // per gestire gli eventuali errori della ricezione
    memset(&(client->name), 0, sizeof(client->name));
   
    if(recv(client->socket,&(client->name),sizeof(client->name),0)<0)    //ricezione nome client
        perror("dati non ricevuti");
    printf("[%s]: e' entrato nella chatroom!!\n",client->name);
 printf("PUNTATORE:%p     NOME:%s  PNOME:%p SOCK:%d\n",c,client->name,&(client->name),client->socket);
    char client_response[256] ;
	while(1){
        if(flag =recv(client->socket,&client_response,sizeof(client_response),0)>0){
            pthread_mutex_lock(&mutexLog);
            fprintf(fdLog,"[%s]: %s",client->name,client_response);
            fflush(fdLog);
            printf("[%s]: %s",client->name,client_response);
            pthread_mutex_unlock(&mutexLog);
            
        }

        if(flag==-1)//errore di ricezione
            perror("errore di ricezione");

        if(flag==0)//client disconesso   
            break;
            
            
    }
      sendtoAll();
    close(client->socket);
    pthread_mutex_lock(&mutexLog);
    fprintf(fdLog,"%s ha lasciato la stanza\n",client->name);
    fflush(fdLog);
    pthread_mutex_unlock(&mutexLog);
    printf("%s ha lasciato la stanza\n",client->name);
    
    if(client->prev==&root){//vuol dire che e il primo
        client->next->prev =&root;
        root.next = client->next;
    }
    /*if(client->prev==NULL)//vuol dire che e il primo
        client->next->prev =NULL;*/
    else if(client->next==NULL)//vuol dire che e l ultimo
        client->prev->next=NULL;
    else{//vuool dire che sta in mezzo
        client->prev->next = client->next;
        client->next->prev=client->prev;
    }
    printf("########################\n");
    sendtoAll();
    free(client);
}


int main(int argc, char* argv[]){

    if(argc<2){
        printf("porta non inserita!");
       // return -1;
    }

    //inizializzazione mutex per il file di log    
    pthread_mutex_init(&mutexLog,NULL);
    
    //tempo e inzializzazione file di log
    time_t rawtime;
    struct tm *info;
    time( &rawtime );
    info = localtime( &rawtime );
    char stringa[14];
    sprintf(stringa,"log %d-%d.txt",info->tm_mon,info->tm_mday);//un file per giorno
    fdLog = fopen(stringa,"a+");
    if(fdLog==NULL)
        perror("file non aperto/creato");
    fprintf(fdLog,"\nsessione: %s\n",asctime(info));
    printf("Current local tim and date: %s", asctime(info));

    //creazione socket e gestione errore
    int server;
    server = socket(AF_INET,SOCK_STREAM,0);

    if(server==-1)
        perror("socket non creato\n");

    //Struttura del socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(argv[1]));
    server_address.sin_addr.s_addr =INADDR_ANY;
    
    //bind socket con struttura e gestion errore
    if(bind(server,(struct sockaddr*) &server_address,sizeof(server_address))==-1)
        perror("binding non riuscito");

     
    //ascolto delle connessioni e gestione e rrore
    if(listen(server,CLIENT)==-1)
        perror("socket non in ascolto");
   printf("qua\n");
    
    struct client *node;
   
    //accettazione connessioni
    while(1){
        //struttura socket dei client
        struct client *client = malloc(sizeof(struct client));
        
//printf("CLIENT:%p\n",client);
        //costruisco una catena
        if(root.next==NULL){//root restera sempore lo stesso
            root.next = client;
            node = client;
            client->prev = &root;
           // printf("null\n");
        }
        else{//cambia ad ogni while, lego il client precedente al client che ho appena creato
           
            node->next = client;
            client->prev = node;
            node = client;
           
            client->next = NULL;
             //printf("node;%p node.next; %p   client:%p",node,node->next,&client); //SEMBRA GIUSTO NODE = A CLIENTE
            
        }    
        
        //inizializzazione comunicazione e struttura client
        printf("waiting clients...\n");
	    client->socket = accept(server,NULL,NULL); 
       printf("CLIENT SOCKET:%d",client->socket);
        if(send(client->socket,WELCOME,sizeof(WELCOME),0)==-1)    //messaggio di benevenuto
            perror("messaggio non inviato");

        //thread per la gestione delle ricezione dei client
        pthread_t tid;
        pthread_create(&tid,NULL,receive,client);
    }
    fclose(fdLog);
    pthread_mutex_destroy(&mutexLog);
    printf("file chiuso");


}
