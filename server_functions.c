#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int get_from_client(int sockfd, char *buffer) {
    int len;
    char *tmp = calloc(9,1); 

    // firstly read number of bytes (always 3 digits)
    if (read(sockfd,tmp,8) < 0)
        return -1;

    // we recieved number length of next message
    if (sscanf(tmp, "%d", &len) != 1) {
        free(tmp);
        return -1;
    }
    free(tmp);

    if (read(sockfd,buffer,len) < 0)
        return -1;
    buffer[len] = '\0';

    return 1;
}

int setup_socket(int portno) {
    struct sockaddr_in serv_addr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
       error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
             error("ERROR on binding");
    listen(sockfd,5);
    return sockfd;
}
