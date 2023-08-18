#include "command.h"
#include "tokstr.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#ifdef _MSC_VER
#define IS_SAME(a, b) (_strcmpi(a, b) == 0)
#else
#include <strings.h>
#define IS_SAME(a, b) (strcasecmp(a, b) == 0)
#endif

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

Command* token_command(const char* cmd) {
    size_t token_len;
    char **tokens = tokstr((char *) cmd, " ", &token_len);

    Command *c = malloc(sizeof(Command));

    if (c == NULL) {
        return NULL;
    }

    c->pin = NOPIN;
    c->state = OFF;
    c->label = (char*)malloc(sizeof(char) * MAX_LABEL);
    memset(c->label, 0, sizeof(char) * MAX_LABEL);

    for (size_t iter = 0; iter < token_len; iter++) {
        if (iter == 0) {
            if (IS_SAME(tokens[iter], "SET")) {
                c->instruction = SET;
            }
            else if (IS_SAME(tokens[iter], "SHOW")) {
                c->instruction = SHOW;
            }
            else if (IS_SAME(tokens[iter], "REBOOT")) {
                c->instruction = REBOOT;
                return c;
            }
            else if (IS_SAME(tokens[iter], "TOGGLE")) {
                c->instruction = TOGGLE;
            }
            else if (IS_SAME(tokens[iter], "LABEL")) {
                c->instruction = LABEL;
            }
            else {
                return NULL;
            }
        }
        else if (iter == 1 && !IS_SAME(tokens[iter], "PIN")) {
            return NULL;
        }
        else if (iter == 2){
            if (*tokens[iter] >= '2' && *tokens[iter] <= '9') {
                c->pin = *tokens[iter] - '0';
            }
            else if (IS_SAME(tokens[iter], "ALL")) {
                c->pin = ALL;
            }
            else {
                return NULL;
            }
        }
        else if (iter == 3) {
            if (IS_SAME(tokens[iter], "ON") || IS_SAME(tokens[iter], "HIGH")) {
                c->state = ON;
            }
            else if (IS_SAME(tokens[iter], "OFF") || IS_SAME(tokens[iter], "LOW")) {
                c->state = OFF;
            }
            else {
                strncat(c->label, tokens[iter], strlen(tokens[iter]));
            }
        }
        else if (iter > 3) {
            strncat(c->label, tokens[iter], strlen(tokens[iter]));
        }
    }

    free(tokens);
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