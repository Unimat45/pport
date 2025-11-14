#ifndef COMMAND_H
#define COMMAND_H

#include <stdint.h>
#include <stddef.h>

#define MAX_ERR_LEN 39

typedef struct Parallel *lpParallel;

typedef enum {
    NotSet = 0,
    Show,
    Set,
    Toggle,
    Label,
    Timings,
    DeleteTimings,
    DeleteTiming,
    Last = DeleteTiming
} Action;

typedef struct {
    Action action;
    uint8_t pin;
    void *payload;
    size_t payload_size;
} AST;

int command_parse(void *cmd, size_t cmd_size, AST *restrict ast, char **errMsg);
size_t command_exec(AST *ast, lpParallel port, void *restrict data, char **errMsg);

#endif // !COMMAND_H
