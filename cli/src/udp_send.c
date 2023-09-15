#include "udp_send.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXLINE 1024

char *udp_send(const char *const host, const uint16_t port, const char *const cmd) {
	int sockfd;
    struct sockaddr_in servaddr;
   
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        return NULL;
    }
   
    memset(&servaddr, 0, sizeof(servaddr));
       
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(host);
       
    int n;
    socklen_t len;
       
    n = sendto(sockfd, cmd, strlen(cmd), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    
    if (n < 0) {
        return NULL;
    }

    char *buffer = malloc(sizeof(char) * MAXLINE);
    n = recvfrom(sockfd, buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);

    if (n < 0) {
        return NULL;
    }

    buffer[n] = '\0';
   
    close(sockfd);

    return buffer;
}