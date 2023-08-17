#ifndef COMMAND_H
#define COMMAND_H

#include "parallel.h"

typedef enum {
    SET = 0,
    SHOW,
    REBOOT,
    TOGGLE,
    LABEL
} Instruction;

typedef enum {
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
    PinState state;
    char *label;
} Command;

Command* parse_command(const char* cmd);

void free_command(Command *cmd);

#endif // COMMAND_H