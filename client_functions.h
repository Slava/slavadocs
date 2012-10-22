// setup socket connection to server
// with hostname and port number
int setup_socket(char *hostname, int portno);
// send message to server by socket description
void send_to_server(int sockfd, const char *msg);
// fail with message
void error(const char *msg);
