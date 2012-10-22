CC = gcc
CFLAGS = -Wall -O2

all: server client client_gui

server:
	$(CC) $(CFLAGS) server.c -o server

client: client_functions.o
	$(CC) $(CFLAGS) client.c client_functions.o -o client

client_gui:
	$(CC) $(CFLAGS) -o client_gui client_gui.c client_functions.o -export-dynamic `pkg-config --cflags --libs gtk+-3.0`

client_functions:
	$(CC) $(CFLAGS) -c client_functions

clean:
	rm -rf server client client_gui *.o

