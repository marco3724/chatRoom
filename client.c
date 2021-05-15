#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#define SIZE 256


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
	char server_response[SIZE];
	int f = 0;
	if(recv(client,&server_response,sizeof(server_response),0)<0)
		perror("messaggio non ricevuto");
	printf("%s ",server_response);
	fflush(stdout);
	
	char name[SIZE];
	char buf; //buffer(inutile) per togliere \n dallo stdin
	scanf("%s",&name);
	scanf("%c",&buf); //serve per togliere '\n' 
    if(send(client,name ,strlen(name),0)==-1)
		 perror("messaggio non inviato");
		 
		
	
    //invio
	char msg[256];
	while(1){
		
		//per bellezza
		printf("\r%s", "> ");
    	fflush(stdout);
	  
    	fgets(msg,SIZE,stdin);

		//uscire dalla chatroom con il comando /quit
		if(strcmp(msg,"/quit\n")==0)
			break;
		//inviare il messaggio
    	if(send(client,msg ,256,0)==-1)
    		perror("messaggio non inviato");
	}

	//chiusura client
	close(client);
	printf("sei uscito/a dalla stanza\n");

	return 0;


}

