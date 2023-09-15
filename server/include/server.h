#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>

int initialize_server(uint16_t port);
void start_server(void);

#endif // SERVER_H