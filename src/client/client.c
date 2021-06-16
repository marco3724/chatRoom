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
#include "./headers/defaultClient.h"

void* receive(void* c){
	int client = *(int *)c;//file descriptor client
	char server_response[MES_SIZE+NAME_SIZE+DATA_SIZE+PADDING+5];//rispopsta del server
	int flag;//flag di controllo

	while(1){
		//se riceve un messaggio lo printa sullo stdout
		if((flag = recv(client,&server_response,sizeof(server_response),0)>0)){
			printf("\r%s",server_response);//stampa la risposta
			fflush(stdout);
			printf("\r%s", "(io): ");
    		fflush(stdout);
		}
		//se 0 allora disconesso dal server
		if(flag==0){
			printf("\rdisconnesso dal server\n");
			exit(EXIT_FAILURE);
		}
	}
}

int main(int argc,char* argv[]){
	if(argc<2)
        perror("porta non inserita!");

	//creazione socket
	int client = socket(AF_INET,SOCK_STREAM,0);
	if(client==-1){
		perror("errore creazione client");
		exit(EXIT_FAILURE);
	}

	//struttura al quale connettersi
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[1]));  
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
	
	//connessione e gestione errore
	int connection_status = connect(client,(struct sockaddr *) &server_addr,sizeof(server_addr));
	if(connection_status== -1){
		perror("errore di connessione\n");
		exit(EXIT_FAILURE);
	}
	printf("client connesso sulla porta:%d\n",atoi(argv[1]));
	printf("scrivere /quit per uscire\n");
	//inizializzazione comunicazione(scambio di messaggi:benevenuto(server) e nome(client))
	startCommunication(client);

	//thread che gestisce la ricezione	 	
	pthread_t tid;
    pthread_create(&tid,NULL,receive,&client);


    //invio
	char msg[MES_SIZE];//messaggio puro
	char fullMsg[MES_SIZE+DATA_SIZE+PADDING];//messaggio con data e bit che rappresenta il numero di caratteri di data
	while(1){
		memset(fullMsg,0,MES_SIZE+DATA_SIZE+PADDING);//inizializzo il messaggio

		//formatto per bellezza
		printf("\r%s", "(io): ");
    	fflush(stdout);
	  	
		//input
    	fgets(msg,MES_SIZE,stdin);
		
		//cancella l'ultima riga di quello che ho scritto
		printf("\033[A\33[2K");
		fflush(stdout);

		//uscire dalla chatroom con il comando /quit
		if(strcmp(msg,"/quit\n")==0)
			break;
		//impacchetto il messaggio
		pack(fullMsg,msg);
		//invio il messaggio completo
    	if(send(client,fullMsg,MES_SIZE+DATA_SIZE+PADDING,0)==-1)
    		perror("messaggio non inviato"); 
	}
	//chiusura client
	close(client);
	printf("sei uscito/a dalla stanza\n");
	return 0;
}

