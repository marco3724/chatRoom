#include "defaultClient.h"	
#include <stdio.h>  
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>    
#include <stdlib.h>
#include <unistd.h> 
#include <time.h>
    
    
 void startCommunication(int client){   
    char server_response[MES_SIZE];
	
	if(recv(client,&server_response,sizeof(server_response),0)<0)
		perror("messaggio non ricevuto");
	printf("%s ",server_response);
	fflush(stdout);
	
	char name[NAME_SIZE];
	fgets(name,NAME_SIZE,stdin);
	
    if (name[strlen(name)-1] == '\n') //formattazione
           name[strlen(name)-1] = '\0';

    if(send(client,name ,strlen(name),0)==-1)
		 perror("messaggio non inviato");
 }


 void pack(char* fullMsg,char*msg){
	 
	 struct tm* info = getCurrentTime();
	 char dateInfo[DATA_SIZE];
		memset(dateInfo,'\0',sizeof(dateInfo));
		sprintf(dateInfo,"%d:%d:%d",info->tm_hour,info->tm_min,info->tm_sec);
		int c1 =0;
		while(dateInfo[++c1]);
		//printf("%d %c\n",c1,dateInfo[c1]);

		sprintf(fullMsg,"%d%s%s",c1,dateInfo,msg);
 }