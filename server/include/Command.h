#ifndef COMMAND_H
#define COMMAND_H

#include "Parallel.h"

typedef enum {
    SET = 0,
    SHOW,
    TOGGLE,
    LABEL,
    NOINST
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
    std::string label;
} Tokens;

class Command {
private:
    Tokens *ast = nullptr;

    void tokenize(std::string command);

public:
    Command(std::string command);
    ~Command(void);

    std::vector<uint8_t> execute(Parallel *parallel);
};

#endif // COMMAND_H