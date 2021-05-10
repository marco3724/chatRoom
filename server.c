#include <stdio.h>
#include <sys/socket.h>


#include <netinet/in.h>

#include <errno.h>
#define CLIENT 5
#define WELCOME "Benvenuto/a nella chatroom"


int main(int argc, char* argv[]){
    //creazione socket e gestione errore
    int server;
    server = socket(AF_INET,SOCK_STREAM,0);
    if(server==-1)
        perror("socket non creato\n");

    //Struttura del socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(10010);
    server_address.sin_addr.s_addr =INADDR_ANY;
    
    //bind socket con struttura e gestion errore
    if(bind(server,(struct sockaddr*) &server_address,sizeof(server_address))==-1)
        perror("binding non riuscito");
    
    //ascolto delle connessioni e gestione e rrore
    if(listen(server,CLIENT)==-1)
        perror("socket non in ascolto");
   

    //accettazione connessioni
  //  while(1){

        int client;
        struct sockaddr_in client_address;
        int len =sizeof(client_address);

        //printf('waiting...');
	    client = accept(server,(struct sockaddr *)&client_address,&len); 
    while(1){  
        char c[256];
        printf("inserisci un messaggio\n");  
        scanf("%s",&c);
        if(send(client,c ,sizeof(c),0)==-1)
            perror("messaggio non inviato");
        
        char server_response[256];
	    if(recv(client,&server_response,sizeof(server_response),MSG_WAITALL)==-1)
            perror("ricezione non riuscito");
        printf("%s\n",server_response);


    }


}
