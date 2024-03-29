#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include "client_functions.h"


void error(const char *msg) {
    perror(msg);
    exit(-1);
}

void send_to_server(int sockfd, const char *msg) {
    // lets send number of bytes we want to send
    int length = strlen(msg);
    char *len_text = calloc(9, 1);

    sprintf(len_text, "%08d", length);
    if (write(sockfd, len_text, 8) < 0)
        error("ERROR writing");
    
    free(len_text);

    // and only then send this number of bytes
    if (write(sockfd, msg, length) < 0)
        error("ERROR writing");
}

int setup_socket(char *hostname, int portno) {
    struct hostent *server;
    struct sockaddr_in serv_addr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "ERROR opening socket\n");
        return -1;
    }
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        return -1;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        fprintf(stderr, "ERROR connecting\n");
        return -1;
    }
    
    return sockfd;
}
