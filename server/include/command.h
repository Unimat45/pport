#ifndef COMMAND_H
#define COMMAND_H

#include "parallel.h"
#include <stdint.h>

typedef enum {
    SET = 0,
    SHOW,
    REBOOT,
    TOGGLE,
    LABEL
} Instruction;

typedef enum {
    NOPIN = 0,
    PIN2 = 2,
    PIN3,
    PIN4,
    PIN5,
    PIN6,
    PIN7,
    PIN8,
    PIN9,
    ALL = 0xFF
} Pins;

typedef struct {
    Instruction instruction;
    Pins pin;
    uint8_t state;
    char label[261];
} Command;

void free_command(Command *cmd);
void *parse_command(Pin parallel[8], Command* c, size_t *len);
unsigned char token_command(Command *c, char* cmd);

#endif // COMMAND_H
