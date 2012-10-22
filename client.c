// Client in the internet domain using TCP
// port number and host are passed as arguments
// communicates with server by passing commands

// C includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "client_functions.h"

int main(int argc, char *argv[]) {
    int sockfd, portno;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port [kill]\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);

    sockfd = setup_socket(argv[1], portno);
    bzero(buffer,256);

    if (argc > 3 && !strcmp(argv[3], "kill")) {
        send_to_server(sockfd, "please terminate");
        close(sockfd);
        return 0;
    }

    send_to_server(sockfd, "initiate session");

    // while we get input transfer to server
    while (gets(buffer+1)) {
        // we do type request
        buffer[0] = 't';
        send_to_server(sockfd, buffer);
    }

    // close socket
    close(sockfd);

    return 0;
}

