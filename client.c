#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <errno.h>
#include "defaultClient.h"


void* receive(void* c){
	int client = *(int *)c;
	char server_response[MES_SIZE];
	
	while(1){
		if(recv(client,&server_response,sizeof(server_response),0)>0){
			printf("\r%s",server_response);
			fflush(stdout);
			printf("\r%s", "(io): ");
    		fflush(stdout);
		}
	}

}








int main(int argc,char* argv[]){
	if(argc<2)
        perror("porta non inserita!");

	//creazione socket
	int client = socket(AF_INET,SOCK_STREAM,0);
	
	//struttura al quale connettersi
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
   
	server_addr.sin_port = htons(atoi(argv[1]));  //host to network short
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
	
	//connessione e gestione errore
	int connection_status = connect(client,(struct sockaddr *) &server_addr,sizeof(server_addr));
	if(connection_status== -1)
		perror("errore di connessione\n");
	
	
	//ricezione
	char server_response[MES_SIZE];
	//printf("%s",asctime(getCurrentTime()));
//	struct tm* info = getCurrentTime();
	//char time[10];
//	sprintf(time,"%d:%d:%d",info->tm_hour,info->tm_min,info->tm_sec);
	//printf("%s\n",time);





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
		 
		
	pthread_t tid;
    pthread_create(&tid,NULL,receive,&client);
    //invio
	char msg[MES_SIZE];
	char fullMsg[MES_SIZE+DATA_SIZE+PADDING];
	while(1){
		memset(fullMsg,0,MES_SIZE+DATA_SIZE+PADDING);
		//per bellezza
		printf("\r%s", "(io): ");
    	fflush(stdout);
	  	
    	fgets(msg,MES_SIZE,stdin);
		//info = getCurrentTime();

		printf("\033[A\33[2K");
		fflush(stdout);
	

		//uscire dalla chatroom con il comando /quit
		if(strcmp(msg,"/quit\n")==0)
			break;
pack(fullMsg,msg);
		//char dateInfo[DATA_SIZE];
		//memset(dateInfo,'\0',sizeof(dateInfo));
		//sprintf(dateInfo,"%d:%d:%d",info->tm_hour,info->tm_min,info->tm_sec);
	//	int c1 =0;
		//while(dateInfo[++c1]);
		//printf("%d %c\n",c1,dateInfo[c1]);

		//sprintf(fullMsg,"%d%s%s",c1,dateInfo,msg);
		//printf("%s%s",msg,fullMsg);
		//sprintf(); MANDO SIZENOME SIZEDATA NOME DATA MESSAGGIO
		//inviare il messaggio
    	if(send(client,fullMsg,MES_SIZE+DATA_SIZE+PADDING,0)==-1)
    		perror("messaggio non inviato"); 
	}
	//chiusura client
	close(client);
	printf("sei uscito/a dalla stanza\n");
	

	return 0;


}

