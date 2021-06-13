#include "defaultServer.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <string.h>

int returnPort(int argc,char* argv[]){
    if(argc<2)
        return PORT;
    else
        return atoi(argv[1]);
}

FILE* folderSettings(char filePath[],struct tm *info){
    char cartella[strlen(DIR)+15];
    sprintf(cartella,"./%s/%d-%d-%d",DIR,info->tm_year-100,info->tm_mon+1,info->tm_mday);

    if(mkdir(DIR,S_IRWXU | S_IRWXG)==-1 && errno!=EEXIST)
        perror("errore creazione cartella");

    if(mkdir(cartella,S_IRWXU | S_IRWXG)==-1 && errno!=EEXIST)
        perror("errore creazione cartella");

       
    sprintf(filePath,"%s/%s",cartella,DIR_CLIENTS);
   
    if(mkdir(filePath,S_IRWXU | S_IRWXG)==-1 && errno!=EEXIST)
        perror("errore creazione cartella");
    
    //size del nome del file di log del server (cartellaprincipale/data(10)/server.txt)
    int size = strlen(DIR) +strlen(SERVER_LOGFILE_NAME) +15;

    char file[size];
    sprintf(file,"%s/%s",cartella,SERVER_LOGFILE_NAME);
    return fopen(file,"a+");

}


 void logAndPrint(char* name,char* message,char* color,char* client_response,FILE *serverLog,FILE *clientLog){
    int size =NAME_SIZE+3+strlen(message)+2;
    char formattedName[NAME_SIZE+3] ;
    char finalMsg[size];
    if(strcmp(color,WHITE)==0)
        sprintf(formattedName,"[%s]:",name);
    else
        sprintf(formattedName,"%s",name);
    if(message[strlen(message)-1]== '\n')
        message[strlen(message)-1] = '\0';

    sprintf(finalMsg,"%s %s\n",formattedName,message);

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
    memset(msg,0,totSize);
    memset(infoDate,0,DATA_SIZE);
    int i;
    int offset = client_response[0]-'0';
    
    for( i=0;i<totSize-1;i++){
        if(i<offset)
            infoDate[i] = client_response[i+1];
        else
            msg[i-offset] = client_response[i+1];
    }

       
            
 }
//store

//getmessage
 //LOG AND FORMAT
 //Send to all