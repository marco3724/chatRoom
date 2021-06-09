#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>



#include <netinet/in.h>
#include <string.h>

#include <pthread.h>
#include <errno.h>
#include <time.h>

#include "defaultServer.h"
#include "structClient.h"
#include "structQueue.h"


pthread_mutex_t mutexLog; //lock per accedere al file di log
FILE *serverLog; //file di log dei messaggi
int mode = 0;




//il path per raggiungere i log dei client cartellaPrincipale/data/cartellaclients
char filePath[SIZE_DIR_CLIENTS]; 


//ricezione di messaggi dai client
void* receive(void* c){
    struct client *client= (struct client*)c;
    
    int flag ;   // per gestire gli eventuali errori della ricezione
    memset(&(client->name), 0, sizeof(client->name));
   
    if(recv(client->socket,&(client->name),sizeof(client->name),0)<=0){    //ricezione nome client
        perror("nome non ricevuto, disconessione");
        close(client->socket);
        removeNode(client);
        free(client);
        return 0;
        }
    
    char clientFileName[SIZE_DIR_CLIENTS+sizeof(client->name)+DATA_SIZE];
    sprintf(clientFileName,"%s/%s.txt",filePath,client->name);
    client->log = fopen(clientFileName,"a+");
    char client_response[MES_SIZE] ;

    pthread_mutex_lock(&mutexLog);

    logAndPrint(client->name,JOIN_MESSAGE,GREEN,client_response,serverLog,client->log);
    sendtoAll(client,&client_response);
    
    pthread_mutex_unlock(&mutexLog);

    char fullMsg[FULL_MEXSIZE];
    int size = strlen(client->name)+3;
    char formattedName[size];
    sprintf(formattedName,"[%s]:",client->name);

	while(1){
        if((flag =recv(client->socket,&client_response,sizeof(client_response),0))>0){

            pthread_mutex_lock(&mutexLog);
            
            logAndPrint(formattedName,client_response,WHITE,fullMsg,serverLog,client->log);
            sendtoAll(client,fullMsg);
    
            pthread_mutex_unlock(&mutexLog);
 
        }

        if(flag==-1)//errore di ricezione
            perror("errore di ricezione");

        if(flag==0)//client disconesso   
            break;     
    }
     
    
    
    pthread_mutex_lock(&mutexLog);

    logAndPrint(client->name,EXIT_MESSAGE,RED,client_response,serverLog,client->log);
    sendtoAll(client, client_response);
    close(client->socket);
    removeNode(client);
    pthread_mutex_unlock(&mutexLog);
    
    free(client);
    return 0;
}
void* broadcast(){
return 0;
}

int main(int argc, char* argv[]){

    int port = returnPort(argc,argv);
    //inizializzazione mutex per il file di log  

    pthread_mutex_init(&mutexLog,NULL);
   
  

    //tempo e inzializzazione file di log
    struct tm *info = getCurrentTime();

    //crea 2 cartelle : la cartella logFile e all interno un altra cartella client
    serverLog = folderSettings(filePath,info);
    if(serverLog==NULL)
        perror("file non aperto/creato");

    fprintf(serverLog,"\nsessione: %s\n",asctime(info));
    fflush(serverLog);
    //printf("Current local tim and date: %s", asctime(info));

    //creazione socket e gestione errore
    int server;
    server = socket(AF_INET,SOCK_STREAM,0);

    if(server==-1)
        perror("socket non creato\n");

    
    //Struttura del socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr =INADDR_ANY;
    
    //bind socket con struttura e gestion errore
    if(bind(server,(struct sockaddr*) &server_address,sizeof(server_address))==-1)
        perror("binding non riuscito");

     
    //ascolto delle connessioni e gestione e rrore
    if(listen(server,CLIENT)==-1)
        perror("socket non in ascolto");
  
    struct coda *queue = malloc(sizeof(struct coda));
    initQueue(queue);
    printf("%d",queue->start);



    struct client *node = &root;
   
    //accettazione connessioni
    while(1){
        //struttura socket dei client
         struct client *client = malloc(sizeof(struct client));
        
        //aggiunge il client appena creato alla catena e setta node = client
         addNode(client,&node);

        
        //inizializzazione comunicazione e struttura client
         printf("waiting clients...\n");
	     client->socket = accept(server,NULL,NULL);

        if(send(client->socket,WELCOME,sizeof(WELCOME),0)==-1)    //messaggio di benevenuto
            perror("messaggio non inviato");

        //thread per la gestione delle ricezione dei client
        pthread_t tid;
        pthread_create(&tid,NULL,receive,client);
    }
    fclose(serverLog);
    pthread_mutex_destroy(&mutexLog); 
    free(filePath);
    free(node);
    free(info);
    free(&server_address);
    
    printf("file chiuso");
    return 0;

}
