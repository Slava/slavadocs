all: server client

server:
	g++ server.cpp -o server -Wall

client:
	g++ client.cpp -o client -Wall

clean:
	rm -rf server client *.o
