#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <netinet/in.h>
#include <string.h>

#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

#include "./headers/defaultServer.h"
#include "./headers/structClient.h"
#include "./headers/structQueue.h"


pthread_mutex_t mutexLog; //lock per accedere al file di log
FILE *serverLog; //file di log dei messaggi
int mode = MODE;//modalita -t(time stamp del client) o -r(ricezione del server)
struct coda *queue; //coda dei messaggi

//il path per raggiungere i log dei client cartellaPrincipale/data/cartellaclients
char filePath[SIZE_DIR_CLIENTS]; 

//usata dai thread per ricevere
void* receive(void* c);
//usato da un thread per avviare comandi (in questo caso solo uno:/quit)
void* commands(void *n);

//usato da un thread per il broadcast dei messaggi in coda
void* broadcast(void *n);

//usato per gestire SIG INT
void handler();

int main(int argc, char* argv[]){

    int port= PORT; //porta di default
    checkParams(argc,argv,&port,&mode);//controllo i parametri passati sullo stdin, se presenti vengono assegnati alla porta e alla modalita
    printf("%s",ISTRUCTION);
    queue = malloc(sizeof(struct coda)); //alloco della memoria per la coda
    initQueue(queue); //inizializzo la coda
    pthread_mutex_init(&mutexLog,NULL); //inizializzo mutex

    //tempo e inzializzazione file di log
    struct tm *info = getCurrentTime();

    //crea 2 cartelle : la cartella logFile e all interno un altra cartella client e restituisce il puntatore al file di log del server
    serverLog = folderSettings(filePath,info);
    if(serverLog==NULL){
        perror("file non aperto/creato");
         exit(EXIT_FAILURE);
    }

    //scrivo sul file di log del server l'ora della sessione corrente
    fprintf(serverLog,"\nsessione: %s\n",asctime(info));
    fflush(serverLog);
    

    //creazione socket e gestione errore
    int server;
    server = socket(AF_INET,SOCK_STREAM,0);
    if(server==-1){
        perror("socket non creato\n");
        exit(EXIT_FAILURE);
    }
 
    //Struttura del socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET; //famiglia di indirizzi IPV4
    server_address.sin_port = htons(port); //porta
    server_address.sin_addr.s_addr =INADDR_ANY; //qualsiasi interfaccia
    
    //bind socket con struttura e gestion errore
    if(bind(server,(struct sockaddr*) &server_address,sizeof(server_address))==-1){
        perror("binding non riuscito");
        exit(EXIT_FAILURE);
    }
     
    //ascolto delle connessioni e gestione e rrore
    if(listen(server,CLIENT)==-1){
        perror("socket non in ascolto");
        exit(EXIT_FAILURE);
    }
    printf("server in ascolto sulla porta: %d e modalita':%d\n",port,mode);
    
   
    //unico thread che gestisce la coda e  l'invio di messaggi
    pthread_t sendTid;
    pthread_create(&sendTid,NULL,broadcast,NULL);

    //thread per gestire i comandi(solo /quit)
    pthread_t commandTid;
    pthread_create(&commandTid,NULL,commands,NULL);

    struct client *node = &root; //assegno root al nodo node(rappresenta l'ultimo nodo della catena)
    signal(SIGINT,handler);//gestisco il segnale SIGINT

    //accettazione connessioni
    while(1){
       
        struct client *client = malloc(sizeof(struct client));//alloco memoria per il nuovo client
        
        //aggiunge il client appena creato alla catena e setta node = client (come ultimo nodo della catena)
        addNode(client,&node);

        //inizializzazione comunicazione e struttura client
        printf("\rwaiting clients...\n");
        printf("\r> ");
        fflush(stdout);
	    client->socket = accept(server,NULL,NULL);
        if(client->socket==-1)
            perror("accettazione socket client fallito");
       
        if(send(client->socket,WELCOME,sizeof(WELCOME),0)==-1)    //messaggio di benevenuto
            perror("messaggio non inviato");

        //thread per la gestione delle ricezione dei client
        pthread_t receiveTid;
        if(pthread_create(&receiveTid,NULL,receive,client)!=0){
            perror("errore nel creare il thread");
            exit(EXIT_FAILURE);
        }
    }

    //non arrivera' mai qui
    return 0;
}

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
        close(client->socket);//chiude il socket
        removeNode(client);//toglie il nodo dalla catena
        free(client);//disalloca il client
        return 0;
    }
    
   
    sprintf(clientFileName,"%s/%s.txt",filePath,client->name);//costruzione percordo del file del client
    client->log = fopen(clientFileName,"a"); //puntatore al file del client
   
    //blocco per scrivere che e' arrivato un client e lo invio a tutti
    pthread_mutex_lock(&mutexLog);
    stringifyCurrentTime(infoDate);
    logAndFormat(client->name,JOIN_MESSAGE,GREEN,infoDate,client_response,serverLog,client->log);
    sendtoAll(client, client_response);
    pthread_mutex_unlock(&mutexLog);

    //scambio di messaggi
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
    

   //blocco per scrivere che e' uscito un client lo invio a tutti
    pthread_mutex_lock(&mutexLog);
    stringifyCurrentTime(infoDate);
    logAndFormat(client->name,EXIT_MESSAGE,RED,infoDate,client_response,serverLog,client->log);
    sendtoAll(client, client_response);
    close(client->socket);
    removeNode(client);
    pthread_mutex_unlock(&mutexLog);
   
    free(client);//libero il client
    return 0;
}

//funzione del thread che gestisce comandi
void* commands(void *n){
    char cmd[MES_SIZE];
    while(1){
		
		printf("\r%s", "> ");
    	fflush(stdout);
	  	
    	fgets(cmd,MES_SIZE,stdin);//input

		//uscire dalla chatroom con il comando /quit
		if(strcmp(cmd,"/quit\n")==0)
			break;
    }
    char time[DATA_SIZE];
    //thread per scrivere al server log
    pthread_mutex_lock(&mutexLog);
    stringifyCurrentTime(time);
    fprintf(serverLog,"%s fine sessione\n",time);
    pthread_mutex_unlock(&mutexLog);

    //routine di chiusura
    fclose(serverLog);
    pthread_mutex_destroy(&mutexLog); 
    free(queue);
    printf("server chiuso correttamente\n");
    exit(EXIT_SUCCESS);
    	
}

//invia il primo messaggio in coda a tutti i nodi
void* broadcast(void *n){
    struct message *msg;
    char fullMsg[FULL_MEXSIZE];
    time_t info;
    time(&info);
    time_t sec1 = info;
    while(1){
       
        if(mode==1 && info-sec1>=SEND_SECONDS+SLEEP_SECONDS) {// se -t (1), ha SEND_SECONDS secondi per inviare i messaaggi
            printf("\raccumulo messaggi\n");//print di accumulo
            sleep(SLEEP_SECONDS);//accumula per SLEEP_SECONDS(3) secondi i messaggi
            sort(queue);//ordino al coda
            sec1 = info; //aggiorno il tempo
           
        }
        msg = getMessage(queue);//restituisce un messaggio
        time(&info);//aggiorno il tempo
        if(strcmp("",msg->client->name)==0){//vuol dire che il client si e' disconnesso
            printf("messaggio non inviato perche appartiene ad un client disconeesso\n");
            continue;
        }
        pthread_mutex_lock(&mutexLog);//lock per scrivere su file e invio di messaggi a tutta la catena di client
         
        logAndFormat(msg->client->name,msg->txt,msg->color,msg->time,fullMsg,serverLog,msg->client->log);
        sendtoAll(msg->client,fullMsg);
        pthread_mutex_unlock(&mutexLog);
    }
}

//gestione del segnale SIGINT
void handler(){
    char time[DATA_SIZE];
    pthread_mutex_lock(&mutexLog);//lock
    stringifyCurrentTime(time);
    fprintf(serverLog,"%s sessione interrotta dal segnale ctrl+c \n",time);//scrive sul file
    pthread_mutex_unlock(&mutexLog);//sblocco
    
    //routine di chiusura
    fclose(serverLog);
    pthread_mutex_destroy(&mutexLog); 
    free(queue);
    printf("\nserver chiuso mediante segnale ctrl+c\n");
    exit(EXIT_SUCCESS);
}