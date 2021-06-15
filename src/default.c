#include "default.h"
#include <time.h>
#include <stdio.h>
struct tm * getCurrentTime(){
    time_t rawtime;
    time( &rawtime );
    return  localtime( &rawtime );
}

void stringifyCurrentTime(char *dateInfo){
    struct tm* info = getCurrentTime();
    sprintf(dateInfo,"%d:%d:%d",info->tm_hour,info->tm_min,info->tm_sec);
   
}