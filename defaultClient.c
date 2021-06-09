#include "defaultClient.h"	
#include <stdio.h>  
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>    
#include <stdlib.h>
#include <unistd.h> 
    
    
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