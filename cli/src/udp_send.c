#include "udp_send.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#define MAXLINE 1024

void *udp_send(const char *const host, const uint16_t port, const char *const cmd, size_t *res_len) {
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

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
        close(sockfd);
        return NULL;
    }
       
    int n;
    socklen_t len = sizeof(servaddr);
       
    n = sendto(sockfd, cmd, strlen(cmd), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    
    if (n < 0) {
        close(sockfd);
        return NULL;
    }

    void *buffer = malloc(MAXLINE);

    if (buffer == NULL) {
        close(sockfd);
        return NULL;
    }

    n = recvfrom(sockfd, buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);

    if (n < 0) {
        close(sockfd);
        free(buffer);
        return NULL;
    }

    if (res_len != NULL) {
        *res_len = n;
    }
   
    close(sockfd);

    return buffer;
}
