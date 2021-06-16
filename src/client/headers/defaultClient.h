#ifndef DEFAULT_CLIENT_H
#define DEFAULT_CLIENT_H
#include "../../default.h"

//inizializza la connessione fra client e server scambiandosi due messaggi: il server invia un messaggio di benvenuto, e il client invia il nome
void startCommunication(int);

//impachetta il messaggio msg aggiungendo un bit + ora in fullMsg, serviranno al server
void pack(char* fullMsg,char*msg);
#endif
