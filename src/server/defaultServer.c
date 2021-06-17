#include "./headers/defaultServer.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <string.h>

void checkParams(int argc,char* argv[],int *port,int* mode){
    for(int i = 1;i<argc && i<3;i++){
        if(argv[i][0]=='-'){ //se inizia con - , allora indica il mode
            if(argv[i][1]=='r')//setta mode ricezione del server
                *mode = 0;
            else if(argv[i][1]=='t')//setta mode timestamp di invio
                *mode = 1;
            else{
                perror("parametro non valido");
                exit(EXIT_FAILURE);
            }
        }
         else if(argv[i][0]>='0' && argv[i][0]<='9')//controlla se la prima cifra e' un numero
             *port = atoi(argv[i]);
    }
         
            

}


FILE* folderSettings(char filePath[],struct tm *info){
    char cartella[strlen(DIR)+15];
    sprintf(cartella,"./%s/%d-%d-%d",DIR,info->tm_year-100,info->tm_mon+1,info->tm_mday);//creo il percorso per i file di log del giorno corrente

    if(mkdir(DIR,S_IRWXU | S_IRWXG)==-1 && errno!=EEXIST){//creo la cartella del giorno per i log
        perror("errore creazione cartella");
        exit(EXIT_FAILURE);
    }

    if(mkdir(cartella,S_IRWXU | S_IRWXG)==-1 && errno!=EEXIST){//crea la cartella
        perror("errore creazione cartella");
        exit(EXIT_FAILURE);
    }

    sprintf(filePath,"%s/%s",cartella,DIR_CLIENTS);//crea il percorso della cartella dei clients
   
    if(mkdir(filePath,S_IRWXU | S_IRWXG)==-1 && errno!=EEXIST){//crea la cartella dei clients
        perror("errore creazione cartella");
        exit(EXIT_FAILURE);
    }
    
    //size del nome del file di log del server (cartellaprincipale/data(10)/server.txt)
    int size = strlen(DIR) +strlen(SERVER_LOGFILE_NAME) +15;

    char file[size];
    sprintf(file,"%s/%s",cartella,SERVER_LOGFILE_NAME);//creo il percordo del file di log del server
    return fopen(file,"a");//ritorno il puntatore al file di log del server

}


 void logAndFormat(char* name,char* message,char* color,char* time,char* client_response,FILE *serverLog,FILE *clientLog){
    int size =NAME_SIZE+3+strlen(message)+2;
    char formattedName[NAME_SIZE+3] ;
    char finalMsg[size];

    //formatto il nome 
    if(strcmp(color,WHITE)==0)
        sprintf(formattedName,"[%s]:",name);//bianco vuol dire messaggio normale
    else
        strcpy(formattedName,name);//altri colori sono messaggi di uscita e non hanno bisgono di formattazione

    if(message[strlen(message)-1]== '\n')//formatto il messaggio
        message[strlen(message)-1] = '\0';

    sprintf(finalMsg,"%s %s %s\n",time,formattedName,message);//costruisco il messaggio finale con il nome formattato e il messaggio

    if(fprintf(serverLog,"%s",finalMsg)<0)//scrittura sul file di log server
        perror("errore scrittura log file di server\n");
    fflush(serverLog);

    if(fprintf(clientLog,"%s",finalMsg)<0)//scrittura sul file ddi log client
        perror("errore scrittura log file di client\n");
    fflush(clientLog);

    sprintf(client_response,"%s%s%s",color,finalMsg,WHITE); //formattazione per invio
   
 }


 void unpack(char* infoDate,char* msg,char* client_response){
    int totSize = MES_SIZE+DATA_SIZE+PADDING; 
    memset(msg,0,totSize);//inizializzo 
    memset(infoDate,0,DATA_SIZE);//inizializzo 
    
    
    int offset = client_response[0]-'0';//converto il primo carattere in intero e lo usero' come offset
    
    for( int i=0;i<totSize-1;i++){
        if(i<offset)
            infoDate[i] = client_response[i+1];//i primi (offset) caratteri corrispondono all'ora di INVIO del messaggio
        else
            msg[i-offset] = client_response[i+1];//il resto e' il messaggio dell'utente
    }           
 }
