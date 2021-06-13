#ifndef DEFAULT_SERVER_H
#define DEFAULT_SERVER_H

#include "default.h"
#include <stdio.h>

//SERVER SETTINGS
#define FULL_MEXSIZE MES_SIZE+NAME_SIZE+DATA_SIZE+PADDING+5
#define CLIENT 5

//SERVER DEFAULT MESSAGES
#define QUEUE_SIZE 20
#define WELCOME "\033[0;36m =====Benvenuto/a nella chatroom=====\n\033[0;37m inserisci il tuo nome(max 20 caratteri):"
#define EXIT_MESSAGE "ha lasciato la stanza"
#define JOIN_MESSAGE "e' entrato/a nella stanza!!"

//COLORS
#define RED "\033[0;31m"
#define WHITE  "\033[0;37m"
#define GREEN "\033[0;36m"


//SERVER FOLDER
#define SERVER_LOGFILE_NAME "server.txt"
#define DIR "logFile"
#define DIR_CLIENTS  "clients"

struct message{
    struct client *client;
    char time[DATA_SIZE];
    char txt[MES_SIZE];
    char* color;
};


#define SIZE_DIR_CLIENTS 8+DATA_SIZE+8+5




int returnPort(int,char*[]);

//inserisco in msg il messaggio, l'ora in infodata da clietn response
void unpack(char* ,char* ,char* );
FILE* folderSettings(char[],struct tm*);
void logAndPrint( char*,char* ,char* ,char* ,FILE *,FILE*);
#endif