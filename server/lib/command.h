#ifndef COMMAND_H
#define COMMAND_H

#include <stdint.h>
#include <stddef.h>

#include "parallel.h"
#include "pport_export.h"

#define MAX_ERR_LEN 39

typedef enum {
    NotSet = 0,
    Show,
    Set,
    Toggle,
    Label,
    Timings,
    NextTrigger,
    DeleteAllTimings,
    DeleteTiming,
    Last = DeleteTiming
} Action;

typedef struct {
    Action action;
    uint8_t pin;
    void *payload;
    size_t payload_size;
} AST;

PPORT_EXPORT int command_parse(void *cmd, size_t cmd_size, AST *restrict ast, char **errMsg);
PPORT_EXPORT size_t command_exec(AST *ast, Parallel *port, void *restrict data, char **errMsg);

#endif // !COMMAND_H
