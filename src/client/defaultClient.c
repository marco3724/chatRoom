#include "./headers/defaultClient.h"	
#include <stdio.h>  
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>    
#include <stdlib.h>
#include <unistd.h> 
#include <time.h>
    
    
 void startCommunication(int client){   
    char server_response[MES_SIZE];
	memset(server_response,0,sizeof(server_response));//inizializzo

	if(recv(client,&server_response,sizeof(server_response),0)<0){//riceve il messaggio di benvenuto dal server
		perror("messaggio non ricevuto");//stampa l'errore su stdout
		exit(EXIT_FAILURE);//esce
	}
	printf("%s ",server_response);//printa su schermo
	fflush(stdout);
	
	char name[NAME_SIZE];
	memset(name,0,NAME_SIZE);
	
	fgets(name,NAME_SIZE,stdin);//input nome
	if(strlen(name) < 2 || strlen(name) >= NAME_SIZE-1){//se non soddisfa i requisiti coninuo a ciclare
		printf("il nome deve essere almeno lungo 1 e meno di %d\n",NAME_SIZE-1);
		exit(EXIT_FAILURE);
	}
	
    if (name[strlen(name)-1] == '\n') //formattazione
           name[strlen(name)-1] = '\0';
	
    if(send(client,name ,strlen(name),0)==-1){//invia il nome
		 perror("messaggio non inviato");//stampa l'errore su stdout
		 exit(EXIT_FAILURE);//esce
	}
	
 }


 void pack(char* fullMsg,char*msg){
	 
	struct tm* info = getCurrentTime();//calcola il tempo corrente
	char dateInfo[DATA_SIZE];
	memset(dateInfo,'\0',sizeof(dateInfo));//inizializzo la data
	sprintf(dateInfo,"%d:%d:%d",info->tm_hour,info->tm_min,info->tm_sec);//compongo la data 

	int c1 =0; //contatore di quanti caratteri e' composto la data puo' variare da 6-9
	while(dateInfo[++c1]);//conto
	
	sprintf(fullMsg,"%d%s%s",c1,dateInfo,msg);//compongo il messaggio
 }