#ifndef NETWORK_H
#define NETWORK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "str_utils.h"

#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS
#endif

#ifdef WINDOWS
#include <WinSock2.h>
typedef int socklen_t;
#else
typedef int SOCKET;
#include <sys/socket.h>
#include <netinet/in.h>
#endif // WINDOWS

typedef struct {
    SOCKET sock;
    struct sockaddr_in server_addr;
    socklen_t addr_len;
} NetworkID;

int createSocket(const char *addr, uint16_t port, NetworkID *id);
uint8_t sendCommand(const char *cmd, NetworkID *id);
size_t receiveData(Pin **pins, char *error, NetworkID *id);
void freePins(Pin **pins, size_t size);
void closeSocket(NetworkID *id);

#ifdef __cplusplus
}
#endif

#endif // NETWORK_H
