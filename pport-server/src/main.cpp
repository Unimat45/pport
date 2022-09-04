#include <iostream>
#include <sys/io.h>
#include <unistd.h>

#include "Server.h"

#define SERVER_PORT 5663
#define PORT 0x378

int main() {

    if (ioperm(PORT, 1, 1)) {
        std::cout << "Access denied" << std::endl;
        return EXIT_FAILURE;
    }

    if (access("/etc/pport.state", F_OK) == -1) {
        FILE* f = fopen("/etc/pport.state", "w");

        fprintf(f, "%d", 0);

        fclose(f);
    }

    Command reboot("REBOOT");
    reboot.execute();

    Server server(SERVER_PORT);

    server.StartServer();

    server.StopServer();

    return 0;
}