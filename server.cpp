// Server in the internet domain using TCP
// The port number and path to html document
// are passed as arguments

// C includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// C++ includes
#include <stack>
#include <algorithm>

// template for web page
#define WEBPAGE_HEADER "<html><head><title>Web page</title></head><body>"
#define WEBPAGE_FOOTER "</body></html>"

// filename of html web page we use
char *HTML_FILE_NAME;

void run_session(int);
void get_stack(std::stack<char>, char*);

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void setup_socket(int &sockfd, int portno, sockaddr_in &serv_addr) {
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
}

int main(int argc, char *argv[]) {
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 3) {
         fprintf(stderr,"usage: %s <port> <path to webpage>\n", argv[0]);
         exit(1);
     }

     // port number
     portno = atoi(argv[1]);

     // path to html file
     HTML_FILE_NAME = argv[2];

     setup_socket(sockfd, portno, serv_addr);
     clilen = sizeof(cli_addr);

     // server is always ready to accept one connection
     while (true) {
         newsockfd = accept(sockfd,
                 (struct sockaddr *) &cli_addr,
                 &clilen);
         if (newsockfd < 0)
             error("ERROR on accept");
         bzero(buffer,256);
         n = read(newsockfd,buffer,255);
         if (n < 0) error("ERROR reading from socket");

         // server will terminate after special command
         if (!strcmp(buffer, "please terminate")) {
            close(newsockfd);
            break;
         }

         // if session was initiated correctly run
         if (!strcmp(buffer, "initiate session"))
            run_session(newsockfd);

         // else just ignore this connection
         close(newsockfd);
     }

     close(sockfd);
     return 0;
}

void run_session(int sockfd) {
    char buffer[256], command[256], command_attr[256], webpage[256*256];
    // two stacks for work with text
    std::stack<char> left, right;

    // while we receive any text
    while (~read(sockfd,buffer,255)) {
        // add received text to our page
        strcat(webpage, buffer); strcat(webpage, "<br>");

        // and we put all text with HEADER and FOOTER to
        // file defined in HTML_FILE_NAME
        FILE *htmlfile = fopen(HTML_FILE_NAME, "w");
        fprintf(htmlfile, "%s\n%s\n%s\n", WEBPAGE_HEADER, webpage, WEBPAGE_FOOTER);

        fclose(htmlfile);
    }
}


