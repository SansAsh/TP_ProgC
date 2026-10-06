CC ?= gcc

CFLAGS=-Wall -Wextra -g

CC ?= gcc
CFLAGS ?= -Wall -Wextra -Wpedantic -g
COBJS = client.o
SOBJS = serveur.o

.SUFFIXES: .c .o

SERVER = serveur
CLIENT = client

all: $(SERVER) $(CLIENT)

$(SERVER): $(SOBJS)
	$(CC) $(CFLAGS) -o $(SERVER) $(SOBJS)

$(CLIENT): $(COBJS)
	$(CC) $(CFLAGS) -o $(CLIENT) $(COBJS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER) $(CLIENT) $(COBJS) $(SOBJS)

.PHONY: all clean
