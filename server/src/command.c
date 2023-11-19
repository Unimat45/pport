#include "command.h"
#include "parallel.h"
#include "tokstr.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/io.h>

#include <string.h>
#include <strings.h>

#define IS_SAME(a, b) (strcasecmp(a, b) == 0)

#define MAX_ITER 15
#define MAX_LABEL 260

void free_command(Command *cmd) {
    if (!cmd) {
        return;
    }
    if (cmd->label) {
        free(cmd->label);
    }
    free(cmd);
}

unsigned char token_command(Command *c, const char* cmd) {
    if (c == NULL) {
        return 0;
    }

    char *token = __tokstr(cmd, ' ');

    if (token == NULL) {
        goto tok_err;
    }

    c->pin = NOPIN;
    c->state = OFF;
    memset(c->label, 0, MAX_LABEL);

    if (IS_SAME(token, "SET")) {
        c->instruction = SET;
    }
    else if (IS_SAME(token, "SHOW")) {
        c->instruction = SHOW;
    }
    else if (IS_SAME(token, "REBOOT")) {
        c->instruction = REBOOT;
        return 1;
    }
    else if (IS_SAME(token, "TOGGLE")) {
        c->instruction = TOGGLE;
    }
    else if (IS_SAME(token, "LABEL")) {
        c->instruction = LABEL;
    }
    else {
        free(token);
        goto tok_err;
    }

    free(token);
    token = __tokstr(cmd, ' ');

    if (token == NULL || !IS_SAME(token, "PIN")) {
        if (token) { free(token); }
        goto tok_err;
    }

    free(token);
    token = __tokstr(cmd, ' ');

    if (token == NULL) {
        goto tok_err;
    }
    
    if (*token >= '2' && *token <= '9') {
        c->pin = *token - '0';
    }
    else if (IS_SAME(token, "ALL")) {
        c->pin = ALL;
    }
    else {
        free(token);
        goto tok_err;
    }

    token = __tokstr(cmd, ' ');

    if (token == NULL) {
        goto tok_err;
    }

    if (IS_SAME(token, "ON") || IS_SAME(token, "HIGH")) {
        c->state = ON;
    }
    else if (IS_SAME(token, "OFF") || IS_SAME(token, "LOW")) {
        c->state = OFF;
    }
    else {
        strncat(c->label, token, MAX_LABEL);
        strncat(c->label, " ", MAX_LABEL);
    }
    
    free(token);

    while ((token = __tokstr(cmd, ' ')) != NULL) {
        strncat(c->label, token, MAX_LABEL);
        strncat(c->label, " ", MAX_LABEL);
        
        free(token);
    }

    size_t len = strlen(c->label) - 1;
    c->label[len] = 0;

    free(token);
    return 1;

tok_err:
    c = NULL;
    return 0;
}

void *parse_command(Pin parallel[8], Command* c, size_t* len) {
    switch (c->instruction) {
        case SHOW: {
            if (c->pin == NOPIN || c->pin == ALL) {
                return parallel_to_mem(len);
            }
            return pin_to_mem(parallel[c->pin - 2], len);
        }
        case SET: {
            switch (c->pin) {
            // Cannot not have a pin number
            case NOPIN:
                return NULL;
            case ALL:
                outb(0xFF * c->state, PORT);
                write_to_file(parallel);
                return parallel_to_mem(len, parallel);
            default: {
                    uint8_t current_value = inb(PORT);
                    if (c->state) {
                        outb(current_value | (1 << (c->pin - 2)), PORT);
                    }
                    else {
                        outb(current_value & (0xFF - (1 << (c->pin - 2))), PORT);
                    }
                    Pin *p = parallel[c->pin - 2];
                    p->state = c->state;

                    write_to_file();
                    return pin_to_mem(p, len);
                }
            }
        }
        case REBOOT:
            load_parallel_from_file(parallel);
            return parallel_to_mem(len, parallel);
        case TOGGLE: {
            if (c->pin == NOPIN || c->pin == ALL) {
                return NULL;
            }

            uint8_t current_value = inb(PORT);
            Pin *p = parallel[c->pin - 2];

            if (p->state) {
                outb(current_value & (0xFF - (1 << (c->pin - 2))), PORT);
            }
            else {
                outb(current_value | (1 << (c->pin - 2)), PORT);
            }

            p->state = !p->state;
            write_to_file(parallel);
            return pin_to_mem(p, len);
        }
        case LABEL: {
            if (c->pin == NOPIN || c->pin == ALL) {
                return NULL;
            }

            Pin *p = parallel[c->pin - 2];

            (void)strncpy(p->label, c->label, MAX_LABEL - 1);

            write_to_file(parallel);
            return pin_to_mem(p, len);
        }
        default:
            break;
    }

    return NULL;
}
