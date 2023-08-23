#include <stdio.h>
#include <sys/io.h>

#include "server.h"

#define IOPORT 0x378
#define SERVER_PORT 5663

int main(void) {
    if (ioperm(IOPORT, 1, 1)) {
        fprintf(stderr, "Access denied\n");
        return 1;
    }

    int errno = initialize_server(SERVER_PORT);

    if (errno != 0) {
        (void)fprintf(stderr, "Error while starting server. Exiting...\n");
        return 1;
    }

    (void)printf("Server started on port %d\n", SERVER_PORT);
    start_server();

    return 0;    
}