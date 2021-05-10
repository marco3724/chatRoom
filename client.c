#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


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
		printf("errore di connessione\n");
	
	while(1){
	//ricezione
	char server_response[SIZE];
	if(recv(client,&server_response,sizeof(server_response),MSG_WAITALL)==-1)
        perror("ricezione non riuscito");

    printf("the server said:%s\n",server_response);

    //invio
	char c[256];
	  // printf("inserisci un messaggio\n");  
        scanf("%s",&c);
    if(send(client,c ,sizeof(c),0)==-1)
            perror("messaggio non inviato");
		printf("messaggio inviato\n");
	}
	//printf("the server said:%s\n",server_response);
	close(client);

	return 0;


}