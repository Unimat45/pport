#include "command.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <stdio.h>

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

Command* parse_command(const char* cmd) {
    char *next_token;
    char *token =	strtok_r((char *)cmd, " ", &next_token);
    uint8_t iter = 0;

    Command *c = malloc(sizeof(Command));
    c->label = (char*)malloc(sizeof(char) * MAX_LABEL);

    while (token != NULL && ++iter < MAX_ITER) {
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
                printf("Ints\n");
                return NULL;
            }
        }
        else if (iter == 2 && !IS_SAME(token, "PIN")) {
            printf("Pin\n");
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
                printf("Nbr\n");
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
                (void)strcat_s(c->label, strlen(token), token);
            }
        }
        else if (iter > 3) {
            (void)strcat_s(c->label, strlen(token), token);
        }

        token = strtok_r(NULL, " ", &next_token);
    }

    return c;
}