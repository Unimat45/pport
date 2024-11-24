#include "command.h"
#include "config.h"
#include "parallel.h"
#include <stdlib.h>
#include <string.h>


#define min(a, b) (a < b ? a : b)

static char err_pinNbr[] = "The pin number must be between 2 and 9";
static char err_payloadMem[] = "Not enough memory";
static char err_wrongInstruction[] = "ERROR: Invalid action";

int command_parse(void *cmd, size_t cmd_size, AST *restrict ast, char **errMsg)
{
    if (cmd == NULL)
        return 0;
    memset(ast, 0, sizeof(AST));

    uint8_t *buf = (uint8_t *)cmd;

    uint8_t action = *buf++;

    if (action > Last)
    {
        action = 0;
    }

    ast->action = (Action)action;

    if (ast->action == Show)
    {
        return 1;
    }

    ast->pin = *buf++;

    if (ast->pin < 2 || ast->pin > 9)
    {
        *errMsg = err_pinNbr;
        return 0;
    }

    size_t capped_size = min(cmd_size - 2, MAX_LABEL);
    ast->payload = malloc(capped_size);

    if (ast->payload == NULL)
    {
        *errMsg = err_payloadMem;
        return 0;
    }

    memcpy(ast->payload, buf, capped_size);
    ast->payload_size = capped_size;

    return 1;
}

size_t command_exec(AST *ast, Parallel *port, void *restrict data,
                    char **errMsg)
{
    size_t ret = 0;

    switch (ast->action)
    {
    case Show:
        ret = parallel_as_mem(port, data);
        break;
    case Set:
    {
        uint8_t pin = ast->pin - 2;
        uint8_t state = *(uint8_t *)ast->payload;
        set_state(port[pin], state);
        ret = parallel_as_mem(port, data);
        config_dump(port);
    }
    break;
    case Label:
    {
        uint8_t pin = ast->pin - 2;
        char *label = (char *)ast->payload;
        set_label(port[pin], label);
        ret = parallel_as_mem(port, data);
        config_dump(port);
    }
    break;
    case Toggle:
    {
        uint8_t pin = ast->pin - 2;
        set_state(port[pin], !port[pin]->state);
        ret = parallel_as_mem(port, data);
        config_dump(port);
    }
    break;
    case Timings:
    {
        uint8_t *payload = (uint8_t *)ast->payload;
        uint8_t pin = ast->pin - 2;
        for (size_t i = 0; i < ast->payload_size; i += TIMING_LEN)
        {
            Timing t;
            t.range = (payload[i] << 24) | (payload[i + 1] << 16) |
                      (payload[i + 2] << 8) | (payload[i + 3]);
            t.hour = (payload[i + 4]);
            t.minute = (payload[i + 5]);
            t.state = (payload[i + 6]);
            t.next = NULL;

            add_timing(port[pin], &t);
        }

        ret = parallel_as_mem(port, data);
        config_dump(port);
    }
    break;
    case DeleteTimings:
    {
        uint8_t pin = ast->pin - 2;

        remove_timings(port[pin]);

        ret = parallel_as_mem(port, data);
        config_dump(port);
    }
    break;
    case DeleteTiming:
    {
        uint8_t pin = ast->pin - 2;

        Timing t;
        memcpy(&t, (char *)ast->payload, TIMING_LEN);
        t.next = NULL;

        remove_timing(port[pin], &t);

        ret = parallel_as_mem(port, data);
        config_dump(port);
    }
    break;
    default:
    case NotSet:
    {
        *errMsg = err_wrongInstruction;
        ret = 0;
    }
    break;
    };

    if (ast->payload)
    {
        free(ast->payload);
    }

    return ret;
}
