CC=gcc
CFLAGS=  -Wall


all: client server

client: client.o defaultClient.o default.o
	$(CC) $(CFLAGS) -pthread client.o defaultClient.o default.o -o client

server: server.o defaultServer.o structClient.o structQueue.o default.o
	$(CC) $(CFLAGS) -pthread server.o defaultServer.o structClient.o structQueue.o default.o -o server

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f *.o 