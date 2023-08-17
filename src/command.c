#include "command.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_ITER 15
#define MAX_LABEL 260
#define IS_SAME(a, b) (_strcmpi(a, b) == 0)

#ifdef _MSC_VER
#define strtok_r strtok_s
#endif

void free_command(Command *cmd) {
    if (!cmd) {
        return;
    }
    if (cmd->label) {
        free(cmd->label);
    }
    free(cmd);
}

Command* token_command(const char* cmd) {
    char *next_token;
    char *token =	strtok_r((char *)cmd, " ", &next_token);
    uint8_t iter = 0;

    Command *c = malloc(sizeof(Command));

    if (c == NULL) {
        return NULL;
    }

    c->pin = NOPIN;
    c->state = OFF;
    c->label = (char*)malloc(sizeof(char) * MAX_LABEL);
    memset(c->label, 0, sizeof(char) * MAX_LABEL);

    while (token != NULL && iter++ < MAX_ITER) {
        if (iter == 1) {
            if (IS_SAME(token, "SET")) {
                c->instruction = SET;
            }
            else if (IS_SAME(token, "SHOW")) {
                c->instruction = SHOW;
            }
            else if (IS_SAME(token, "REBOOT")) {
                c->instruction = REBOOT;
                return c;
            }
            else if (IS_SAME(token, "TOGGLE")) {
                c->instruction = TOGGLE;
            }
            else if (IS_SAME(token, "LABEL")) {
                c->instruction = LABEL;
            }
            else {
                return NULL;
            }
        }
        else if (iter == 2 && !IS_SAME(token, "PIN")) {
            return NULL;
        }
        else if (iter == 3){
            if (*token >= '2' && *token <= '9') {
                c->pin = *token - '0';
            }
            else if (IS_SAME(token, "ALL")) {
                c->pin = ALL;
            }
            else {
                return NULL;
            }
        }
        else if (iter == 4) {
            if (IS_SAME(token, "ON") || IS_SAME(token, "HIGH")) {
                c->state = ON;
            }
            else if (IS_SAME(token, "OFF") || IS_SAME(token, "LOW")) {
                c->state = OFF;
            }
            else {
                strncat(c->label, token, strlen(token));
            }
        }
        else if (iter > 3) {
            strncat(c->label, token, strlen(token));
        }

        token = strtok_r(NULL, " ", &next_token);
    }

    return c;
}

void *parse_command(Command* c) {
    switch (c->instruction) {
        case SHOW: {
            if (c->pin == NOPIN || c->pin == ALL) {
                // TODO: Return JSON for all pins
            }
            break;
        }
        case SET: {
            // Cannot not have a pin number
            if (c->pin == NOPIN) {
                return NULL;
            }

            break;
        }
    }

    return c;
}