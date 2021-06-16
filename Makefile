CC=gcc
CFLAGS=  -Wall
SRC =src/
CLT =$(SRC)client/
SRV = $(SRC)server/
LIB = -pthread

all: client server

client: $(CLT)client.o $(CLT)defaultClient.o $(SRC)default.o
	$(CC) $(CFLAGS) $(LIB) $(CLT)client.o $(CLT)defaultClient.o $(SRC)default.o -o client

server: $(SRV)server.o $(SRV)defaultServer.o $(SRV)structClient.o $(SRV)structQueue.o $(SRC)default.o
	$(CC) $(CFLAGS) $(LIB) $(SRV)server.o $(SRV)defaultServer.o $(SRV)structClient.o $(SRV)structQueue.o $(SRC)default.o -o server

$(SRV)%.o: $(SRV)%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(CLT)%.o: $(CLT)%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC)%.o: $(SRC)%.c
	$(CC) $(CFLAGS) -c $< -o $@



.PHONY: clean

clean:
	rm -f $(SRC)*.o $(SRV)*.o $(CLT)*.o