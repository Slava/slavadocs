// Client in the internet domain using TCP
// port number and host are passed as arguments
// communicates with server by passing commands

// C includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <fcntl.h>
#include <termios.h>

void error(const char *msg) {
    perror(msg);
    exit(0);
}

void setup_socket(int &sockfd, char *hostname, hostent *server, sockaddr_in &serv_addr, int portno) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
}

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    struct termios oldSettings, newSettings;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);

    setup_socket(sockfd, argv[1], server, serv_addr, portno);
    bzero(buffer,256);

    n = write(sockfd, "initiate session", 16);
    if (n < 0)
        error("ERROR writing");

    // while we get input transfer to server
    while (gets(buffer))
        if (write(sockfd, buffer, strlen(buffer)) < 0)
            error("ERROR writing");

    // close socket
    close(sockfd);
    // put settings back
    return 0;
}

