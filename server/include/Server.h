#ifndef SERVER_H
#define SERVER_H

#include <cstdint>

#ifdef _WIN64
typedef size_t SOCKET;
#else
typedef int SOCKET;
#endif

class Server {
private:
    SOCKET server_fd;
    uint16_t port;

public:
    Server(uint16_t port);
    ~Server();

    void startServer(void);
};

#endif // SERVER_H
