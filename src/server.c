#include "server.h"
#include "command.h"

#include <netinet/in.h>
#include <string.h>
#include <strings.h>

#define IS_SAME(a,b) (strcasecmp(a,b) == 0)

#define MAX_BUF 1024

struct sockaddr_in server_adr, client_adr;
int server_fd;

int initialize_server(uint16_t port) {
    load_parallel_from_file();

    server_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (server_fd < 0) {
        return server_fd;
    }

    memset(&server_adr, 0, sizeof(server_adr));
    memset(&client_adr, 0, sizeof(client_adr));

    server_adr.sin_family = AF_INET;
    server_adr.sin_addr.s_addr = INADDR_ANY;
    server_adr.sin_port = htons(port);

    int errno = bind(server_fd, (const struct sockaddr*)&server_adr, sizeof(server_adr));

    if (errno < 0) {
        return errno;
    }

    return 0;
}

void start_server() {
    char buf[MAX_BUF];

    ssize_t len;
    socklen_t msg_len = sizeof(client_adr);

    while (1) {
        len = recvfrom(server_fd, buf, MAX_BUF, MSG_WAITALL, (struct sockaddr *)&client_adr, &msg_len);
        buf[len] = 0;

        if (IS_SAME(buf, "STOP")) {
            break;
        }

        Command *cmd = token_command(buf);

        if (cmd == NULL) {
            sendto(server_fd, "ERROR: Invalid Syntax", 22, MSG_CONFIRM, (const struct sockaddr *)&client_adr, msg_len);
            continue;
        }

        const char *result = parse_command(cmd);

        if (result == NULL) {
            sendto(server_fd, "ERROR: Invalid Syntax", 22, MSG_CONFIRM, (const struct sockaddr *)&client_adr, msg_len);
            continue;
        }

        sendto(server_fd, result, strlen(result), MSG_CONFIRM, (const struct sockaddr *)&client_adr, msg_len);
    }

    shutdown(server_fd, SHUT_RDWR);
}