// Server in the internet domain using TCP
// The port number, path to html document
// and path to HTDOCS are passed as arguments

// C includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>

// template for web page
char *mainwebpage =
"<html>\
    <head>\
    <title></title>\
    <script type=\"text/javascript\">\
        window.onload = startInterval;\
        function startInterval() {\
            setInterval(\"updateText();\",100);\
        }\
\
        function updateText() {\
            xmlhttp=new XMLHttpRequest();\
            xmlhttp.onreadystatechange=function() {\
                if (xmlhttp.readyState==4 && xmlhttp.status==200)\
                    document.getElementById('mainblock').innerHTML = xmlhttp.responseText;\
            }\
            xmlhttp.open(\"GET\",\"slavadocs_data.txt\",true);\
            xmlhttp.send();            \
        }\
    </script>\
    </head>\
    <body>  \
        <div id=\"mainblock\"></div>\
    </body>\
</html>";

// path to html web page we use
char *HTML_FILE_NAME;
// path to temp data storage
char datatxt[256];

void run_session(int);
void normalize(char *);

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
    if (argc < 4) {
        fprintf(stderr,"usage: %s <port> <path to webpage> <path to HTDOCS>\n", argv[0]);
        exit(1);
    }

    // port number
    portno = atoi(argv[1]);

    // path to html file
    HTML_FILE_NAME = argv[2];

    // path to temp data
    bzero(datatxt,256);
    strcat(datatxt, argv[3]);
    strcat(datatxt, "/slavadocs_data.txt");

    // open socket to listen
    setup_socket(sockfd, portno, serv_addr);
    clilen = sizeof(cli_addr);

    // put html file
    FILE *htmlfile = fopen(HTML_FILE_NAME, "w");
    if (!htmlfile)
        error("ERROR opening file");

    fprintf(htmlfile, "%s\n", mainwebpage);
    fclose(htmlfile);

    FILE *datafile = fopen(datatxt, "w");
    fprintf(datafile, "nothing here");

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

    remove(HTML_FILE_NAME);
    remove(datatxt);
    close(sockfd);
    return 0;
}

void run_session(int sockfd) {
    fprintf(stderr, "start of session\n");
    char buffer[256], command[256], command_attr[256], webpage[256*256];
    bzero(buffer,256);
    bzero(webpage, 256*256);

    // while we receive any text
    while (read(sockfd,buffer,255) >= 0) {
        // add received text to our page

        FILE *datafile = fopen(datatxt, "w");

        // check it is type request
        if (*buffer == 't') {
            normalize(buffer+1);
            strcat(webpage, buffer+1); strcat(webpage, "<br>");

            fprintf(stderr, "just read: %s\n", buffer);

            // and we put all text with HEADER and FOOTER to
            // file defined in HTML_FILE_NAME
            fprintf(datafile, "%s\n", webpage);
        } else if (*buffer == 'c') {    // control requests
            ;
        } else {
            // probably something is wrong
            break;
        }

        fclose(datafile);
        bzero(buffer,256);
    }

    fprintf(stderr, "end of session\n");
}

void normalize(char *s) {
    char buffer[256], *p, *t;
    t = s;
    p = buffer;

    bzero(buffer,256);

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
            default:
                *p++ = *t;
        }
        t++;
    }

    strcpy(s, buffer);
}

