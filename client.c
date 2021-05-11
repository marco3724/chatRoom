#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h> 
#define SIZE 256
#define MES "connessione riuscita"

int main(int argc,char* argv[]){
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
	
			perror("messaggio non ricevutop");
	printf("server :%s ",server_response);
	fflush(stdout);
	char c[256];
	scanf("%s",&c);
    if(send(client,c ,strlen(c),0)==-1)
		 perror("messaggio non inviato");
		
		
	
    //invio
	while(1){
		char msg[256];
	    printf("inserisci un messaggio\n");  
	   //fflush(stdout);
       fgets(msg,256,stdin);
        if(send(client,msg ,256,0)==-1)
            perror("messaggio non inviato");
		//printf("messaggio inviato\n");
	}
	
	//printf("the server said:%s\n",server_response);
	close(client);

	return 0;


}

