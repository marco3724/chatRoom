CC=gcc
CFLAGS=  -Wall


all: client server

client: client.o
	$(CC) $(CFLAGS) -pthread client.o -o client

server: server.o default.o structClient.o 
	$(CC) $(CFLAGS) -pthread server.o default.o structClient.o -o server

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f *.o 