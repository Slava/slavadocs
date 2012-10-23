CC = gcc
CFLAGS = -Wall -O2
#CFLAGS += -g

all: server client client_gui

server: server_functions.o
	$(CC) $(CFLAGS) server.c server_functions.o -o server

server_functions.o:
	$(CC) $(CFLAGS) -c server_functions.c

client: client_functions.o
	$(CC) $(CFLAGS) client.c client_functions.o -o client

client_gui: client_functions.o
	$(CC) $(CFLAGS) -o client_gui client_gui.c client_functions.o -export-dynamic `pkg-config --cflags --libs gtk+-3.0`

client_functions.o:
	$(CC) $(CFLAGS) -c client_functions.c

clean:
	rm -f server client client_gui *.o

