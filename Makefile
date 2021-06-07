CC=gcc
CFLAGS=  -Wall

server: server.o default.o structClient.o 
	$(CC) $(CFLAGS) -pthread server.o default.o structClient.o -o server

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f *.o 