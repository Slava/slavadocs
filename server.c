// Server in the internet domain using TCP
// The port number, path to html document
// and path to HTDOCS are passed as arguments

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include "server_functions.h"

#define MAXPAGESIZE (256*256)


// template for web page
const char *mainwebpage =
"<html>\n\
    <head>\n\
    <title></title>\n\
    <script type=\"text/javascript\">\n\
        window.onload = startInterval;\n\
        function startInterval() {\n\
            setInterval(\"updateText();\",100);\n\
        }\n\
\n\
        function updateText() {\n\
            xmlhttp=new XMLHttpRequest();\n\
            xmlhttp.onreadystatechange=function() {\n\
                if (xmlhttp.readyState==4 && xmlhttp.status==200)\n\
                    document.getElementById('mainblock').innerHTML = xmlhttp.responseText;\n\
            }\n\
            xmlhttp.open(\"GET\",\"slavadocs_data.txt?%d\",true);\n\
            xmlhttp.send();            \n\
        }\n\
    </script>\n\
    </head>\n\
    <body>  \n\
        <div id=\"mainblock\"></div>\n\
    </body>\n\
</html>";

// path to html web page we use
char *HTML_FILE_NAME;
// path to temp data storage
char datatxt[MAXPAGESIZE];

void run_session(int);
void normalize(char *);


int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[MAXPAGESIZE];
    struct sockaddr_in cli_addr;
    int n;
    if (argc < 4) {
        fprintf(stderr,"usage: %s <port> <path to webpage> <path to HTDOCS>\n", argv[0]);
        exit(1);
    }

    // port number
    portno = atoi(argv[1]);

    // path to html file
    HTML_FILE_NAME = argv[2];

    // path to temp data
    bzero(datatxt,MAXPAGESIZE);
    strcat(datatxt, argv[3]);
    strcat(datatxt, "/slavadocs_data.txt");

    // open socket to listen
    sockfd = setup_socket(portno);
    clilen = sizeof(cli_addr);

    // put html file
    FILE *htmlfile = fopen(HTML_FILE_NAME, "w");
    if (!htmlfile)
        error("ERROR opening file");

    srand(time(0));
    fprintf(htmlfile, mainwebpage, rand() ^ rand() << 16);
    fclose(htmlfile);


    // server is always ready to accept one connection
    while (1) {
        FILE *datafile = fopen(datatxt, "w");
        fprintf(datafile, "nothing here");
        fclose(datafile);
        newsockfd = accept(sockfd,
                (struct sockaddr *) &cli_addr,
                &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");
        bzero(buffer,MAXPAGESIZE);
        n = get_from_client(newsockfd,buffer);
        if (n < 0) error("ERROR reading from socket");

        // server will terminate after special command
        if (!strcmp(buffer, "please terminate")) {
            fprintf(stderr, "caught terminate signal\n");
            close(newsockfd);
            break;
        }

        // if session was initiated correctly run
        if (!strcmp(buffer, "initiate session"))
           run_session(newsockfd);

        // else just ignore this connection
        close(newsockfd);
    }

    remove(HTML_FILE_NAME);
    remove(datatxt);
    close(sockfd);
    return 0;
}

void run_session(int sockfd) {
    fprintf(stderr, "start of session\n");
    char *buffer = calloc(MAXPAGESIZE, 1), 
        *webpage = calloc(MAXPAGESIZE, 1);

    // while we receive any text
    while (get_from_client(sockfd, buffer) != -1) {
        // add received text to our page

        FILE *datafile = fopen(datatxt, "w");

        fprintf(stderr, "just read: %s\n", buffer);

        // check it is type request
        if (*buffer == 't') {
            normalize(buffer+1);
            strcat(webpage, buffer+1);

            // and we put all text with HEADER and FOOTER to
            // file defined in HTML_FILE_NAME
            fprintf(datafile, "%s\n", webpage);
        } else if (*buffer == 'c') {    // control requests
            if (!strcmp(buffer+1, "clear"))
                bzero(webpage, MAXPAGESIZE);
        } else {
            // probably something is wrong
            break;
        }

        fclose(datafile);
        bzero(buffer,MAXPAGESIZE);
    }

    free(buffer); free(webpage);
    fprintf(stderr, "end of session\n");
}

void normalize(char *s) {
    char *buffer = calloc(strlen(s)*6+1, 1), *p, *t;
    t = s;
    p = buffer;

    while (*t) {
        switch (*t) {
            case '<':
                strcat(p, "&lt;");
                p += 4;
                break;
            case '>':
                strcat(p, "&gt;");
                p += 4;
                break;
            case '&':
                strcat(p, "&amp;");
                p += 5;
                break;
            case '"':
                strcat(p, "&quot;");
                p += 6;
                break;
            case '\t':
                strcat(p, "    ");
                p += 4;
                break;
            case '\n':
                strcat(p, "<br>");
                p += 4;
                break;
            default:
                *p++ = *t;
        }
        t++;
    }

    strcpy(s, buffer);
    free(buffer);
}

