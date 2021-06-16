# per eseguire
```
- make
- ./server [mode -t,-r] [porta: int]
- ./client [porta: int]
```
---
# struttura del progetto 
- ## `src` : sono presenti tutti i file sorgenti
    - ### `client` : sono presenti tutti i file necessari a creare il client
        - `header` : ci sono i corripsettivi header dei file .c
        - `client.c` : file principale del client
        - `defaultClient.c` : funzioni ausiliari al client e impostazioni di default
    - ### `server` : sono presenti tutti i file necessari a creare il server
        - `header` : ci sono i corrispettivi header dei file .c
            - `defaultServer.h` : header file che contiene le impostazioni di default come porta/modalita' e lunghezza messaggi
        - `server.c`
        - `defaultServer.c` funzioni ausiliari al server
        - `structClient.c`
        - `structQueue.c`
    - `default.c` : funzioni ausiliare sia al client che al server
- ## `Makefile` : permette di compilare con il comando make 
    - all : compila tutti i file e genera (client e server) 
    - client : compila i file della cartella client+ default.c e genera client
    - server : compila i file della cartella server+ default.c e genera server
    - clean : rimuove i file .o generate nelle varie cartelle
- _queste cartelle verranno create all'avvio del server la prima volta e servono per mantenere i file di log_
- ## `logFile`
    -  `data` :cartella che contiene i file di log di quel giorno formato  YY:MM:DD
        - `clients` :contiene tutti i client che si sono conessi in questo giorno
            - `client1.txt`
            - `client2.txt`
            - ....
        - `serverLog.txt`
- ogni giorno che verra' avviato il server si creera' una gerarchia di cartelle a partire da `data`
