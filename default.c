#include "default.h"
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

struct tm * getCurrentTime(){
    time_t rawtime;
    time( &rawtime );
    return  localtime( &rawtime );
}

FILE* folderSettings(char filePath[],struct tm *info){
    char cartella[strlen(DIR)+15];
    sprintf(cartella,"./%s/%d-%d-%d",DIR,info->tm_year-100,info->tm_mon+1,info->tm_mday);

    if(mkdir(DIR,S_IWOTH)==-1 && errno!=EEXIST)
        perror("errore creazione cartella");

    if(mkdir(cartella,S_IWOTH)==-1 && errno!=EEXIST)
        perror("errore creazione cartella");

       
    sprintf(filePath,"%s/%s",cartella,DIR_CLIENTS);
   
    if(mkdir(filePath,S_IWOTH)==-1 && errno!=EEXIST)
        perror("errore creazione cartella");
    
    //size del nome del file di log del server (cartellaprincipale/data(10)/server.txt)
    int size = strlen(DIR) +strlen(SERVER_LOGFILE_NAME) +15;

    char file[size];
    sprintf(file,"%s/%s",cartella,SERVER_LOGFILE_NAME);
    return fopen(file,"a+");

}


 void logAndPrint(char* name,char* message,char* color,char* client_response,FILE *serverLog,FILE *clientLog){
   int size = strlen(name)+strlen(message)+2;
   char finalMsg[size];

   if(message[strlen(message)-1]== '\n')
        message[strlen(message)-1] = '\0';

   sprintf(finalMsg,"%s %s\n",name,message);

   if(fprintf(serverLog,"%s",finalMsg)<0)//scrittura sul file di log server
        perror("errore scrittura log file di server\n");
    fflush(serverLog);

    if(fprintf(clientLog,"%s",finalMsg)<0)//scrittura sul file ddi log client
        perror("errore scrittura log file di client\n");
    fflush(clientLog);

    printf("%s%s%s",color,finalMsg,WHITE); //stampa stdout

    sprintf(client_response,"%s%s%s",color,finalMsg,WHITE); //formattazione per invio
   
 }