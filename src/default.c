#include "default.h"
#include <time.h>
#include <stdio.h>
struct tm * getCurrentTime(){
    time_t rawtime;
    time( &rawtime );//ritorna l'ora corrente
    return  localtime( &rawtime );// ritorna l'ora corrente sottoforma dui struct tm
}

void stringifyCurrentTime(char *dateInfo){
    struct tm* info = getCurrentTime();//ora corrente
    sprintf(dateInfo,"%d:%d:%d",info->tm_hour,info->tm_min,info->tm_sec);//salva la stringa in infodate
   
}