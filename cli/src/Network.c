#include "Network.h"

#ifndef WINDOWS
#include <arpa/inet.h>
#include <unistd.h>
#else
#include <WS2tcpip.h>
#endif

#include <time.h>
#include <stdlib.h>
#include <string.h>

#define NULLCHECK(a) if (a == NULL) return 0

int createSocket(const char *addr, uint16_t port, NetworkID *id) {
    NULLCHECK(id);

#ifdef WINDOWS
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        return 0;
    }
#endif

    id->sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (id->sock == -1) return 0;

    id->addr_len = sizeof(id->server_addr);

    memset(&id->server_addr, 0, id->addr_len);

    char addr_buf[sizeof(struct in_addr)];
    inet_pton(AF_INET, addr, addr_buf);
    int addr_nbr = *(int*)addr_buf;

    id->server_addr.sin_family = AF_INET;
    id->server_addr.sin_port = htons(port);
    id->server_addr.sin_addr.s_addr = addr_nbr;


    // 1 sec timeout
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    //if (setsockopt(id->sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv,sizeof(tv)) < 0) {
    //    closeSocket(id);
    //    return 0;
    //}

    return 1;
}

uint8_t sendCommand(const char *cmd, NetworkID *id) {
    NULLCHECK(id);

    size_t sent_len = sendto(id->sock, cmd, strnlen(cmd, 1024), 0, (const struct sockaddr*)&id->server_addr, sizeof(id->server_addr));
    return sent_len != -1;
}

size_t receiveData(Pin **pins, char *error, NetworkID *id) {
    NULLCHECK(pins);
    NULLCHECK(id);

    uint8_t buf[1024];
    size_t received = recvfrom(id->sock, (char*)buf, sizeof(buf), 0, (struct sockaddr*)&id->server_addr, &id->addr_len);

    if (memcmp(buf, "ERROR", 5) == 0) {
        memcpy(error, buf, received);
        return 0;
    }

    // is array
    if (*buf) {
        size_t last_buf_idx = 1;

        for (uint8_t i = 0; i < 8; i++) {
            Pin *p = malloc(sizeof(Pin));
            NULLCHECK(p);
            p->state = *(buf + last_buf_idx);
        
            size_t label_len = strlen((char*)buf + last_buf_idx + 1);
            p->label = malloc(label_len + 1);
            if (p->label == NULL) {
                free(p);
                return i;
            }
            memcpy((void*)p->label, buf + last_buf_idx + 1, label_len + 1);

            pins[i] = p;

            // length of label + state + string NULL terminator
            last_buf_idx += label_len + 2;

            if (last_buf_idx > received) {
                return i;
            }
        }

        return 8;
    }
    else {
        Pin *p = malloc(sizeof(Pin));
        NULLCHECK(p);
        p->state = *(buf + 1);
        
        size_t label_len = strlen((char*)buf + 2);
        p->label = malloc(label_len + 1);
        if (p->label == NULL) {
            free(p);
            return 0;
        }

        memcpy((void*)p->label, buf + 2, label_len + 1);

        *pins = p;
        return 1;
    }

}

void freePins(Pin **pins, size_t size) {
    for (size_t i = 0; i < size; i++) {
        Pin *p = pins[i];

        if (p != NULL) {
            if (p->label != NULL) free((void*)p->label);
            free(p);
        }
    }
}

void closeSocket(NetworkID *id) {
#ifdef WINDOWS
    closesocket(id->sock);
    WSACleanup();
#else
    close(id->sock);
#endif
}
