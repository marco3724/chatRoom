#ifndef DEFAULT_H
#define DEAFULT_H

#define PORT 9000
#define MES_SIZE 256
#define FULL_MEXSIZE MES_SIZE+NAME_SIZE+5



#define CLIENT 5
#define WELCOME "\033[0;36m =====Benvenuto/a nella chatroom=====\n\033[0;37m inserisci il tuo nome(max 20 caratteri):"
#define NAME_SIZE 20
#define EXIT_MESSAGE "ha lasciato la stanza"
#define JOIN_MESSAGE "e' entrato/a nella stanza!!"


#define RED "\033[0;31m"
#define WHITE  "\033[0;37m"
#define GREEN "\033[0;36m"

#define SERVER_LOGFILE_NAME "server.txt"
#define DIR "logFile"
#define DIR_CLIENTS  "clients"
#define SIZE_DIR_CLIENTS 8+10+8+5
//#define DIR_SIZE 20    //lunghezza cartella principale
//#define DIR_CLIENT_SIZE DIR_SIZE+10    //lunghezza cartella che contiene i client (il percorso e cartellaprincipale/data)
#include <stdio.h>


int returnPort(int,char*[]);
struct tm * getCurrentTime();
FILE* folderSettings(char[],struct tm*);
void logAndPrint( char*,char* ,char* ,char* ,FILE *,FILE*);
#endif