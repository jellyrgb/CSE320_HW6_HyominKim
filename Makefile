CC=gcc
CFALGS=-g

all: server client

client: client.c util.c
	$(CC) $(CFLAGS) -o client client.c util.c

server: server.c util.c
	$(CC) $(CFLAGS) -o server server.c util.c

clean:
	rm client server
