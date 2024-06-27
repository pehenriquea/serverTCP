CC = gcc
CFLAGS = -Wall

all: server send_client display_client

server: server.c
	$(CC) $(CFLAGS) -o server server.c

send_client: send_client.c
	$(CC) $(CFLAGS) -o send_client send_client.c

display_client: display_client.c
	$(CC) $(CFLAGS) -o display_client display_client.c

clean:
	rm -f server send_client display_client
