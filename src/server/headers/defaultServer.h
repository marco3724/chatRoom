#ifndef DEFAULT_SERVER_H
#define DEFAULT_SERVER_H

#include "../../default.h"
#include <stdio.h>

//SERVER SETTINGS
#define FULL_MEXSIZE MES_SIZE+NAME_SIZE+DATA_SIZE+PADDING+5
#define CLIENT 5 //numero coda listen
#define MODE 0//0 messaggi in ordine di ricezione(-r), 1 = messaggi in ordine di timestamp(-t invio del client)
#define SEND_SECONDS 3 //tempo a disposizione per inviare
#define SLEEP_SECONDS 3 //server per accumulare i messaggi


//SERVER DEFAULT MESSAGES
#define QUEUE_SIZE 20 
#define WELCOME "\033[0;36m =====Benvenuto/a nella chatroom=====\n\033[0;37m inserisci il tuo nome(max 20 caratteri):"
#define EXIT_MESSAGE "ha lasciato la stanza"
#define JOIN_MESSAGE "e' entrato/a nella stanza!!"
#define ISTRUCTION "\033[0;36m########ISTRUZIONI#######\n\033[0;37mserver con due modalita:\n-ricezione(-r) :0 [default]\n-timestamp(-t):1\nscrivere /quit per uscire\n\033[0;36m#########################\033[0;37m\n"

//COLORS
#define RED "\033[0;31m"
#define WHITE  "\033[0;37m"
#define GREEN "\033[0;36m"


//SERVER FOLDER
#define SERVER_LOGFILE_NAME "server.txt"
#define DIR "logFile"
#define DIR_CLIENTS  "clients"
#define SIZE_DIR_CLIENTS 8+DATA_SIZE+8+5 //lunghezza stringa della cartella dei client

//struttura del messaggio
struct message{
    struct client *client; //client che ha inviato
    char time[DATA_SIZE]; //ora di invio
    char txt[MES_SIZE];//testo
    char* color;//colore
};


//cicla tra i parametri, e se soddisfano i requisiti setta port e mode
void checkParams(int argc,char* argv[],int* port,int* mode);

//inserisco in msg il messaggio, l'ora in infodata da clietn response
void unpack(char* ,char* ,char* );

/*crea tre cartelle(solo la prima volta poi due) una cartella principale 
e all'interno ci sara' un altra cartella che avra' 
come nome il giorno di oggi e all'interno ci saranno i file di log del server e un altra cartella con i che conterra i file di log dei client*/
FILE* folderSettings(char[],struct tm*);

//scrivo nei file di log, e formatto il messaggio per inviarlo a tutti
void logAndFormat(char* name,char* message,char* color,char* time,char* client_response,FILE *serverLog,FILE *clientLog);
#endif