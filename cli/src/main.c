#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "Network.h"
#include "argparse.h"
#include "str_utils.h"

#ifndef SERV_ADDR
#define SERV_ADDR "127.0.0.1"
#endif

#ifndef SERV_PORT
#define SERV_PORT 5663
#endif

static const char *const usages[] = {
    "pport [options]",
    NULL,
};

void prettyPrint(Pin **parallel, size_t size) {
    size_t longest = findLongestLabel(parallel, size);

    for (size_t i = 0; i < size; i++) {
        Pin *p = parallel[i];

        size_t pad_len = longest - utf8_strlen(p->label);

        if (pad_len) {
            char padding[260] = {0};
            memset(padding, ' ', pad_len);

            printf("%s%s: %s\n", p->label, padding, (p->state ? "On" : "Off"));
        } else {
            printf("%s: %s\n", p->label, (p->state ? "On" : "Off"));
        }
    }
}

int main(int argc, char **argv) {
#ifdef WINDOWS
    // Set the console to ouput utf-8
    SetConsoleOutputCP(CP_UTF8);
#endif

    bool status = false;
    bool reboot = false;
    int on = 0;
    int off = 0;
    int toggle = 0;
    int pin = 0;
    char *label = NULL;

    struct argparse_option options[] = {
        OPT_HELP(),

        OPT_BOOLEAN(0, "reboot", &reboot, "Resets all pins to their last state",
                    NULL, 0, 0),
        OPT_BOOLEAN(0, "status", &status, "Shows all the pins and their state",
                    NULL, 0, 0),
        OPT_INTEGER('n', "on", &on, "Sets the provided pin high", NULL, 0, 0),
        OPT_INTEGER('f', "off", &off, "Sets the provided pin low", NULL, 0, 0),
        OPT_INTEGER('t', "toggle", &toggle, "Toggles the provided pin", NULL, 0,
                    0),
        OPT_STRING('l', "label", &label, "Sets a label to a provided pin", NULL,
                   0, 0),
        OPT_INTEGER('p', "pin", &pin, "To use with label, specifies the pin",
                    NULL, 0, 0),

        OPT_END()};

    NetworkID id;
    if (!createSocket(SERV_ADDR, SERV_PORT, &id)) {
        fprintf(stderr, "Create socket error\n");
        return 1;
    }

    if (argc < 2) {

        while (true) {
            printf("> ");

            char line[1024] = {0};

            read_line(line);

            if (istrcmp(line, "QUIT")) {
                goto end;
            }

            if (!sendCommand(line, &id)) {
                goto send_err;
            }

            if (istrcmp(line, "STOP")) {
                break;
            }

            Pin *parallel[8];
            char error[512] = {0};
            size_t size = receiveData(parallel, error, &id);

            if (size == 0) {
                fprintf(stderr, "%s\n", error);
            } else {
                prettyPrint(parallel, size);
            }

            freePins(parallel, size);
        }

        goto end;
    }

    struct argparse argparse;
    (void)argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse,
                      "\nThis program is used to control parallel port data "
                      "pins on a UNIX machine",
                      "\nAvailable pin numbers are: 2, 3, 4, 5, 6, 7, 8, 9");
    argc = argparse_parse(&argparse, argc, (const char **)argv);

    if (status) {
        if (!sendCommand("SHOW", &id)) {
            goto send_err;
        }

        Pin *parallel[8];
        char error[512];
        size_t size = receiveData(parallel, error, &id);

        if (size == 0) {
            fprintf(stderr, "%s\n", error);
        } else {
            prettyPrint(parallel, size);
        }
        freePins(parallel, size);
        goto end;
    }

    if (reboot) {
        if (!sendCommand("REBOOT", &id)) {
            goto send_err;
        }

        Pin *parallel[8];
        char error[512];
        size_t size = receiveData(parallel, error, &id);

        if (size == 0) {
            fprintf(stderr, "%s\n", error);
        } else {
            prettyPrint(parallel, size);
        }
        freePins(parallel, size);

        goto end;
    }

    if (on != 0) {
        char cmd[16];
        snprintf(cmd, sizeof(cmd), "SET PIN %d HIGH", on);

        if (!sendCommand(cmd, &id)) {
            goto send_err;
        }

        Pin *parallel[8];
        char error[512];
        size_t size = receiveData(parallel, error, &id);

        if (size == 0) {
            fprintf(stderr, "%s\n", error);
        } else {
            prettyPrint(parallel, size);
        }
        freePins(parallel, size);
    }

    if (off != 0) {
        char cmd[15];
        snprintf(cmd, sizeof(cmd), "SET PIN %d LOW", off);

        if (!sendCommand(cmd, &id)) {
            goto send_err;
        }

        Pin *parallel[8];
        char error[512];
        size_t size = receiveData(parallel, error, &id);

        if (size == 0) {
            fprintf(stderr, "%s\n", error);
        } else {
            prettyPrint(parallel, size);
        }
        freePins(parallel, size);
    }

    if (toggle != 0) {
        char cmd[19];
        snprintf(cmd, sizeof(cmd), "TOGGLE PIN %d", toggle);

        if (!sendCommand(cmd, &id)) {
            goto send_err;
        }

        Pin *parallel[8];
        char error[512];
        size_t size = receiveData(parallel, error, &id);

        if (size == 0) {
            fprintf(stderr, "%s\n", error);
        } else {
            prettyPrint(parallel, size);
        }
        freePins(parallel, size);
    }

    if (label != NULL) {
        if (pin == 0) {
            fprintf(
                stderr,
                "No pin specified! Please specify pin with the --pin option\n");
            goto close_err;
        }

        char cmd[278];
        snprintf(cmd, 278, "LABEL PIN %d %s", pin, label);

        if (!sendCommand(cmd, &id)) {
            goto send_err;
        }

        Pin *parallel[8];
        char error[512];
        size_t size = receiveData(parallel, error, &id);

        if (size == 0) {
            fprintf(stderr, "%s\n", error);
        } else {
            prettyPrint(parallel, size);
        }
        freePins(parallel, size);
    }

end:
    closeSocket(&id);
    return 0;

send_err:
    fprintf(stderr, "Send error: %d\n", errno);
close_err:
    closeSocket(&id);
    return 1;
}
