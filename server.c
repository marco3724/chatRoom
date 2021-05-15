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
};


pthread_mutex_t mutexLog; //lock per accedere al file di log
FILE *fdLog; //file di log dei messaggi
void sendT(){

}
//ricezione di messaggi dai client
void* receive(void* c){
    struct client client= *(struct client*)c;
    
    int flag ;   // per gestire gli eventuali errori della ricezione
    memset(&client.name, 0, sizeof(client.name));

    if(recv(client.socket,&client.name,sizeof(client.name),0)<0)    //ricezione nome client
        perror("dati non ricevuti");
    printf("[%s]: e' entrato nella chatroom!!\n",client.name);

    char client_response[256] ;
	while(1){
        if(flag =recv(client.socket,&client_response,sizeof(client_response),0)>0){
            pthread_mutex_lock(&mutexLog);
            fprintf(fdLog,"[%s]: %s",client.name,client_response);
            fflush(fdLog);
            printf("[%s]: %s",client.name,client_response);
            pthread_mutex_unlock(&mutexLog);
            
        }

        if(flag==-1)//errore di ricezione
            perror("errore di ricezione");

        if(flag==0)//client disconesso
            break;
            
            
    }
      
    close(client.socket);
    pthread_mutex_lock(&mutexLog);
    fprintf(fdLog,"%s ha lasciato la stanza\n",client.name);
    fflush(fdLog);
    pthread_mutex_unlock(&mutexLog);
    printf("%s ha lasciato la stanza\n",client.name);
}


int main(int argc, char* argv[]){

    if(argc<2){
        printf("porta non inserita!");
       // return -1;
    }

    //inizializzazione mutex per il file di log    
    pthread_mutex_init(&mutexLog,NULL);
    
    //tempo e inzializzializzazione file di log
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
    printf("Current local time and date: %s", asctime(info));


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
   

    //accettazione connessioni
    while(1){
        //struttura socket dei client
        struct client client;
        struct sockaddr_in client_address;
        int len =sizeof(client_address);
         //inizializzazione comunicazione e struttura client
        printf("waiting clients...\n");
        fflush(fdLog);
	    client.socket = accept(server,(struct sockaddr *)&client_address,&len); 
       
        if(send(client.socket,WELCOME,sizeof(WELCOME),0)==-1)             //messaggio di benevenuto
            perror("messaggio non inviato");

        //thread per la gestione delle ricezione dei client
        pthread_t tid;
        pthread_create(&tid,NULL,receive,&client);
    }
    fclose(fdLog);
    pthread_mutex_destroy(&mutexLog);
    printf("file chiuso");


}
