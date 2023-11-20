#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>
#include "command.h"

int initialize_server(uint16_t port, Pin parallel[8]);
void start_server(Pin parallel[8]);

#endif // SERVER_H