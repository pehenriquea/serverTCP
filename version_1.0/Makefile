CC = gcc
CFLAGS = -Wall

all: server client_send client_receive

server: server.c
	$(CC) $(CFLAGS) -o server server.c

client_send: client_send.c
	$(CC) $(CFLAGS) -o client_send client_send.c

client_receive: client_receive.c
	$(CC) $(CFLAGS) -o client_receive client_receive.c

clean:
	rm -f server client_send client_receive
