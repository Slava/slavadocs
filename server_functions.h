#pragma once

// put error message and exit(-1)
void error(const char *msg);
// sends buffer to sockfd file descriptor
int get_from_client(int sockfd, char *buffer);
// sets up listen socket and returns fd, accepts port 
int setup_socket(int portno);
