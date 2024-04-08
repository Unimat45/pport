#include "Server.h"
#include "Command.h"
#include "CommandException.hpp"
#include "PinException.hpp"
#include "log.h"
#include "str_utils.h"

#include <cstring>
#include <stdexcept>
#include <string>

#ifdef _WIN64
#include <WS2tcpip.h>
#include <WinSock2.h>

typedef int socklen_t;
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

Server::Server(uint16_t port) : port(port) {
#ifdef _WIN64
    WSADATA wsadata{};
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) == -1) {
        throw std::runtime_error("startup exception");
    }
#endif

    this->server_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (this->server_fd < 0) {
        throw std::runtime_error("socket exception");
    }

    struct sockaddr_in server_addr = {AF_INET, htons(port), INADDR_ANY};

    if (bind(this->server_fd, (const struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        throw std::runtime_error("bind exception");
    }
}

Server::~Server() {
#ifdef _WIN64
    closesocket(this->server_fd);
    WSACleanup();
#else
    close(this->server_fd);
#endif
}

void Server::startServer() {
    log_info("Server started on port %d", this->port);

    Parallel *p = new Parallel;

    while (true) {
        struct sockaddr_in client_addr {};
        socklen_t addr_size = sizeof(client_addr);

        char buf[2048];
        size_t msg_len = recvfrom(this->server_fd, buf, sizeof(buf), 0,
                                  (struct sockaddr *)&client_addr, &addr_size);

        std::string cmd(buf, msg_len);
        trim(cmd);

        if (str_to_upper(cmd) == "STOP") {
            sendto(this->server_fd, "STOP", 4, 0,
                   (const struct sockaddr *)&client_addr, addr_size);

            char addr_buf[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, addr_buf,
                      sizeof(addr_buf));
            log_info("%s: STOP", addr_buf);

            // Stop server execution
            break;
        }

        try {
            Command c(cmd);
            std::vector<uint8_t> res = c.execute(p);
            sendto(this->server_fd, (const char *)res.data(), res.size(), 0,
                   (const struct sockaddr *)&client_addr, addr_size);

            char addr_buf[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, addr_buf,
                      sizeof(addr_buf));
            log_info("%s: %s", addr_buf, cmd.c_str());
        } catch (CommandException &ex) {
            sendto(this->server_fd, ex.what(), strlen(ex.what()), 0,
                   (const struct sockaddr *)&client_addr, addr_size);

            char addr_buf[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, addr_buf,
                      sizeof(addr_buf));
            log_error("%s: %s; %s", addr_buf, ex.what(), cmd.c_str());
        } catch (PinException &ex) {
            sendto(this->server_fd, ex.what(), strlen(ex.what()), 0,
                   (const struct sockaddr *)&client_addr, addr_size);

            char addr_buf[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, addr_buf,
                      sizeof(addr_buf));
            log_error("%s: %s; %s", addr_buf, ex.what(), cmd.c_str());
        }
    }

    delete p;
}
