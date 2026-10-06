CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Werror -g
COBJS = client.o bmp.o couleur.o
SOBJS = serveur.o

.SUFFIXES: .c .o

SERVER = serveur
CLIENT = client

all: $(SERVER) $(CLIENT)

$(SERVER): $(SOBJS)
	$(CC) $(CFLAGS) -o $(SERVER) $(SOBJS) -lm

$(CLIENT): $(COBJS)
	$(CC) $(CFLAGS) -o $(CLIENT) $(COBJS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SERVER) $(CLIENT) $(COBJS) $(SOBJS)

.PHONY: all clean
