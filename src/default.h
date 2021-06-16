#ifndef DEFAULT_H
#define DEFAULT_H

//porta predefinita del server e client
#define PORT 9000

//grandezze dault di stringhe
#define MES_SIZE 200
#define NAME_SIZE 20
#define DATA_SIZE 10  
#define PADDING 10

//resituisce l'ora corrente
struct tm * getCurrentTime();

//salva in dateinfo l'ora corrente sottoforma di stringa
void stringifyCurrentTime(char *dateInfo);
#endif
