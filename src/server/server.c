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
int mode = MODE;
struct coda *queue;



//il path per raggiungere i log dei client cartellaPrincipale/data/cartellaclients
char filePath[SIZE_DIR_CLIENTS]; 


//ricezione di messaggi dai client
void* receive(void* c){
    struct client *client= (struct client*)c; //struttura del client
    char infoDate[DATA_SIZE];//ora dell invio del messaggio dal client
    char clientFileName[SIZE_DIR_CLIENTS+sizeof(client->name)+DATA_SIZE]; //percorso del file del client
    int flag ;   // per gestire gli eventuali errori della ricezione
    char client_response[MES_SIZE+DATA_SIZE+PADDING] ; //risposta(pacchetto[bit+ora+messaggio]) 
    char msg[MES_SIZE+DATA_SIZE+PADDING];//messaggio del client puro( senza ora)

    memset(&(client->name), 0, sizeof(client->name));//inizializzazione nome
   
    if(recv(client->socket,&(client->name),sizeof(client->name),0)<=0){    //ricezione nome client
        perror("nome non ricevuto, disconnessione");//gestione errore
        close(client->socket);
        removeNode(client);
        free(client);
        return 0;
    }
    
   
    sprintf(clientFileName,"%s/%s.txt",filePath,client->name);//costruzione percordo del file del client
    client->log = fopen(clientFileName,"a+"); //puntatore al file del client
   



    stringifyCurrentTime(infoDate);
    storeMessage(queue,JOIN_MESSAGE,infoDate,client,GREEN);//imagazzino nella coda

    
	while(1){
    
        if((flag =recv(client->socket,&client_response,sizeof(client_response),0))>0){//ricezione messaggio
            unpack(infoDate,msg,client_response);//estrazione del messaggio, salva in infoDate l'ora dell'invio del messaggio e in msg il messaggio
            storeMessage(queue,msg,infoDate,client,WHITE);//imagazinamento messaggio nella coda dei messaggi
        }

        if(flag==-1)//errore di ricezione
            perror("errore di ricezione");

        if(flag==0)//client disconesso   
            break;     
    }
    //stringifyCurrentTime(infoDate);
   // storeMessage(queue,EXIT_MESSAGE,infoDate,client,RED);

   
    pthread_mutex_lock(&mutexLog);

    logAndFormat(client->name,EXIT_MESSAGE,RED,client_response,serverLog,client->log);
    sendtoAll(client, client_response);
    close(client->socket);
    removeNode(client);

    pthread_mutex_unlock(&mutexLog);
   
    free(client);
    return 0;
}



//invia il primo messaggio in coda a tutti i nodi
void* broadcast(void *n){
    struct message *msg;
    char fullMsg[FULL_MEXSIZE];

    while(1){
        if(mode) {// se -t (1) allora ogni 3 secondi , riordina i messaggi
            sleep(SLEEP_SECONDS);
            sort(queue);
        }
        msg = getMessage(queue);//restituisce un messaggio

        pthread_mutex_lock(&mutexLog);//lock per scrivere su file e invio di messaggi a tutta la catena di client
        logAndFormat(msg->client->name,msg->txt,msg->color,fullMsg,serverLog,msg->client->log);
        sendtoAll(msg->client,fullMsg);
        pthread_mutex_unlock(&mutexLog);
    }
}


int main(int argc, char* argv[]){

    int port= PORT; //porta di default
    checkParams(argc,argv,&port,&mode);//controllo i parametri passati sullo stdin, se presenti vengono assegnati alla porta e alla modalita

    queue = malloc(sizeof(struct coda)); //alloco della memoria per la coda
    initQueue(queue); //inizializzo la coda
    pthread_mutex_init(&mutexLog,NULL); //inizializzo mutex

   

    //tempo e inzializzazione file di log
    struct tm *info = getCurrentTime();

    //crea 2 cartelle : la cartella logFile e all interno un altra cartella client e restituisce il puntatore al file di log del server
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
  
    
   
    //unico thread che gestisce la coda e  l'invio di messaggi
    pthread_t sendTid;
    pthread_create(&sendTid,NULL,broadcast,NULL);

    struct client *node = &root; //assegno root al nodo node(rappresenta l'ultimo nodo della catena)
   
    //accettazione connessioni
    while(1){
        
        struct client *client = malloc(sizeof(struct client));//alloco memoria per il nuovo client
        
        //aggiunge il client appena creato alla catena e setta node = client (come ultimo nodo della catena)
        addNode(client,&node);

        
        //inizializzazione comunicazione e struttura client
        printf("waiting clients...\n");
	    client->socket = accept(server,NULL,NULL);
        if(client->socket==-1)
            perror("accettazione socket client fallito");

        if(send(client->socket,WELCOME,sizeof(WELCOME),0)==-1)    //messaggio di benevenuto
            perror("messaggio non inviato");

        //thread per la gestione delle ricezione dei client
        pthread_t receiveTid;
        pthread_create(&receiveTid,NULL,receive,client);
    }

    //routine di fine
    fclose(serverLog);
    pthread_mutex_destroy(&mutexLog); 
    free(filePath);
    free(node);
    free(info);
    free(queue);
    free(&server_address);
    
    printf("file chiuso");
    return 0;

}
