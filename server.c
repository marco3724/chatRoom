#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>


#include <netinet/in.h>
#include <string.h>

#include <pthread.h>
#include <errno.h>
#include <time.h>


 #define RED "\033[0;31m"
   #define WHITE  "\033[0;37m"
   #define GREEN "\033[0;36m"
#define CLIENT 5
#define WELCOME "\033[0;36m =====Benvenuto/a nella chatroom=====\n\033[0;37m inserisci il tuo nome(max 20 caratteri):"
#define NAME_SIZE 20
#define DIR "logFile"
//AGGIUNGERE PORTA PREDEFINITA
//AGGIUNGERE SIZE PREDEFINITA E NON

/*struttura di un client
contiene il suo nome e il fd del socket
*/
struct client{
    char name[NAME_SIZE];
    int socket;
    struct client *prev;
    struct client *next ;
};

//radice della catena dei client
struct client root = {"root",-1,NULL,NULL};

pthread_mutex_t mutexLog; //lock per accedere al file di log
FILE *fdLog; //file di log dei messaggi


void sendtoAll(void *c,void *m){
    struct client *client = (struct client*)c;
    char *msg = (char*)m;
    struct client *node = root.next;

    //manda il messaggio a tutti i nodi
    while(node->next !=NULL){
        if(client ==node){
             node = node->next;
             continue; // se node_next e' null ci sarebbe errore, quindi faccio conitnue per rifare il cpontrolllo
        }
        if(send(node->socket,msg ,256,0)==-1)
    		perror("messaggio non inviato");
        
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
    if (client->name[strlen(client->name)-1] == '\n') {
           client->name[strlen(client->name)-1] = '\0';
        }
    char client_response[256] ;
    pthread_mutex_lock(&mutexLog);
    fprintf(fdLog,"%s e' entrato/a nella chatroom!!",client->name);
    pthread_mutex_unlock(&mutexLog);

    sprintf(client_response,"%s%s e' entrato/a nella chatroom!!\n%s",GREEN,client->name,WHITE);
    sendtoAll(client,&client_response);

    printf(client_response);
    char fullMsg[256];
	while(1){
        if(flag =recv(client->socket,&client_response,sizeof(client_response),0)>0){
                
             sprintf(fullMsg,"[%s]: %s",client->name,client_response);
            sendtoAll(client,fullMsg);
            
            pthread_mutex_lock(&mutexLog);
            fprintf(fdLog,"%s",fullMsg);
            fflush(fdLog);
            pthread_mutex_unlock(&mutexLog);

            printf("%s",fullMsg);
           
        }

        if(flag==-1)//errore di ricezione
            perror("errore di ricezione");

        if(flag==0)//client disconesso   
            break;     
    }
     
    
    pthread_mutex_lock(&mutexLog);
    fprintf(fdLog,"%s ha lasciato la stanza\n",client->name);
    fflush(fdLog);
    pthread_mutex_unlock(&mutexLog);

    printf("%s%s ha lasciato la stanza\n%s",RED,client->name,WHITE);
    sprintf(client_response,"%s%s ha lasciato la stanza%s\n",RED,client->name,WHITE);
    
  
    sendtoAll(client, client_response);
    if(client->next == NULL){
        client->prev->next =NULL;
    }
    else if(client->prev==&root ){//vuol dire che e il primo
        client->next->prev =&root;
        root.next = client->next;
    }//DEVO AGGIUNGERE IL FATTO CHE NEXT E' NULL IMPORTANTEEE!!!!!!!!!!!!!!!! CLIENT PREV NETX = NULL METTERLO COME PRIMA CONDIZIONE
    else{//vuool dire che sta in mezzo
        client->prev->next = client->next;
        client->next->prev=client->prev;
    }
    //per come ho strutturato la creazione del client,
    // l'ultimo client della catena non puo' essere NULL ma sara sempre un client che non si e; acnora connesso
    close(client->socket);
    free(client);
}


int main(int argc, char* argv[]){
    if(argc<2){
        printf("porta non inserita!");
       // return -1;
    }

    //inizializzazione mutex per il file di log  

    int counter =0;
    



    pthread_mutex_init(&mutexLog,NULL);
   
    if(mkdir(DIR,S_IWOTH)==-1 )
        perror("errore creazione cartella");





    //tempo e inzializzazione file di log
    time_t rawtime;
    struct tm *info;
    time( &rawtime );
    info = localtime( &rawtime );
    char stringa[14];
    sprintf(stringa,"./%s/%d-%d-%d.txt",DIR,info->tm_year-100,info->tm_mon,info->tm_mday);//un file per giorno
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
  
    
    struct client *node = &root;
   
    //accettazione connessioni
    while(1){
        //struttura socket dei client
        struct client *client = malloc(sizeof(struct client));
        

      
   
        
        //inizializzazione comunicazione e struttura client
        printf("waiting clients...\n");
	    client->socket = accept(server,NULL,NULL);
             node->next = client;
        client->prev = node;
        node = client;
        client->next = NULL;
           
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
