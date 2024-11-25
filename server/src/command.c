#include "command.h"
#include "config.h"
#include "globals.h"
#include "parallel.h"

#ifdef LOG
#include "log.h"
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define min(a, b) (a < b ? a : b)

static char err_pinNbr[] = "The pin number must be between 2 and 9";
static char err_payloadMem[] = "Not enough memory";
static char err_wrongInstruction[] = "ERROR: Invalid action";

void command_tostring(AST *ast, char *cmd)
{
    switch (ast->action)
    {
    case Show:
        // Strcpy is ok here, since static string are always NULL-terminated
        strcpy(cmd, "SHOW");
        break;
    case Set:
        sprintf(cmd, "SET PIN %d %s", ast->pin,
                *((uint8_t *)ast->payload) ? "ON" : "OFF");
        break;
    case Label:
        sprintf(cmd, "LABEL PIN %d %s", ast->pin, (char *)ast->payload);
        break;
    case Toggle:
        sprintf(cmd, "TOGGLE PIN %d", ast->pin);
        break;
    case Timings:
    {
        uint8_t *payload = (uint8_t *)ast->payload;
        uint8_t pin = ast->pin - 2;
        size_t written = 0;
        for (size_t i = 0; i < ast->payload_size; i += TIMING_LEN)
        {
            Timing t;
            t.range = (payload[i] << 24) | (payload[i + 1] << 16) |
                      (payload[i + 2] << 8) | (payload[i + 3]);
            t.hour = (payload[i + 4]);
            t.minute = (payload[i + 5]);
            t.state = (payload[i + 6]);
            t.next = NULL;
            written += sprintf(cmd, "TIMINGS PIN %d %d %s - %d %s %d:%d %s\n",
                               ast->pin, FIRST_DAY(t.range),
                               months[FIRST_MONTH(t.range) - 1], LAST_DAY(t.range),
                               months[LAST_MONTH(t.range) - 1], t.hour, t.minute,
                               t.state ? "ON" : "OFF");
        }

        memset(cmd + written - 1, 0, 1);
    }
    break;
    case DeleteTimings:
        sprintf(cmd, "DELETE ALL TIMINGS FOR PIN %d", ast->pin);
        break;
    case DeleteTiming:
    {
        Timing t;
        memcpy(&t, (char *)ast->payload, TIMING_LEN);
        t.next = NULL;
        sprintf(cmd, "DELETE TIMING FOR PIN %d %d %s - %d %s %d:%d %s",
                ast->pin, FIRST_DAY(t.range), months[FIRST_MONTH(t.range) - 1],
                LAST_DAY(t.range), months[LAST_MONTH(t.range) - 1], t.hour,
                t.minute, t.state ? "ON" : "OFF");
    }
    break;
    default:
        break;
    };
}

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
    memset(ast->payload + capped_size, 0, 1);

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

#ifdef LOG
    {
        char command[1024];
        command_tostring(ast, command);

        log_info("%s", command);
    }
#endif

    if (ast->payload)
    {
        free(ast->payload);
    }

    return ret;
}
