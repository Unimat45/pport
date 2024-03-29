#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <stdlib.h>
#include <json-c/json_object.h>
#include <json-c/json_tokener.h>

#include "argparse.h"
#include "udp_send.h"

#define HOST "10.0.0.106"
#define PORT 5663

static const char *const usages[] = {
    "pport [options]",
    NULL,
};

int findLongestLabel(json_object *arr) {
    int longest = 0;

    for (size_t i = 0; i < json_object_array_length(arr); i++) {
        json_object *p = json_object_array_get_idx(arr, i);

        json_object *label = json_object_object_get(p, "label");

        int l = json_object_get_string_len(label);

        if (l > longest) {
            longest = l;
        }
    }

    return longest;
}

void prettyPrint(json_object *data) {
    if (json_object_is_type(data, json_type_array)) {
        int longest_label = findLongestLabel(data);

        for (size_t i = 0; i < json_object_array_length(data); i++) {
            json_object *p = json_object_array_get_idx(data, i);

            json_object *label = json_object_object_get(p, "label");
            json_object *state = json_object_object_get(p, "state");

            char spaces_buf[1024];
            int length = longest_label - json_object_get_string_len(label);
            
            if (length > 0) {
                (void)memset(spaces_buf, ' ', length - 1);
                spaces_buf[length] = 0;
                (void)printf("%s%s: %s\n", spaces_buf, json_object_get_string(label), json_object_get_boolean(state) ? "on" : "off");
            }
            else {
                (void)printf("%s: %s\n", json_object_get_string(label), json_object_get_boolean(state) ? "on" : "off");
            }
        }
    }
    else {
        json_object *label = json_object_object_get(data, "label");
        json_object *state = json_object_object_get(data, "state");
        (void)printf("%s: %s\n", json_object_get_string(label), json_object_get_boolean(state) ? "on" : "off");
    }
}

int main(int argc, char **argv) {
    bool status = false;
    bool reboot = false;
    int on = 0;
    int off = 0;
    int toggle = 0;
    int pin = 0;
    char *label = NULL;

    struct argparse_option options[] = {
        OPT_HELP(),

        OPT_BOOLEAN(0, "reboot", &reboot, "Resets all pins to their last state", NULL, 0, 0),
        OPT_BOOLEAN(0, "status", &status, "Shows all the pins and their state", NULL, 0, 0),
        OPT_INTEGER('n', "on", &on, "Sets the provided pin high", NULL, 0, 0),
        OPT_INTEGER('f', "off", &off, "Sets the provided pin low", NULL, 0, 0),
        OPT_INTEGER('t', "toggle", &toggle, "Toggles the provided pin", NULL, 0, 0),
        OPT_STRING('l', "label", &label, "Sets a label to a provided pin", NULL, 0, 0),
        OPT_INTEGER('p', "pin", &pin, "To use with label, specifies the pin", NULL, 0, 0),

        OPT_END()
    };

    if (argc < 2) {
        while (true) {
            (void)printf("> ");

            char cmd[1024];
            char *s = fgets(cmd, 1024, stdin);
            (void)s;

            if ((strlen(cmd) > 0) && (cmd[strlen (cmd) - 1] == '\n')) {
                cmd[strlen (cmd) - 1] = 0;
            }

            if (strcasecmp(cmd, "QUIT") == 0) {
                return 0;
            }

            char *res = udp_send(HOST, PORT, cmd);

            if (strstr(res, "ERROR") != NULL) {
                (void)printf("%s\n", res);
            }
            else {
                json_object *data = json_tokener_parse(res);
                prettyPrint(data);
            }

            free(res);
        }
    }

    struct argparse argparse;
    (void)argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\nThis program is used to control parallel port data pins on a UNIX machine", "\nAvailable pin numbers are: 2, 3, 4, 5, 6, 7, 8, 9");
    argc = argparse_parse(&argparse, argc, (const char **)argv);

    if (status) {
        char *res = udp_send(HOST, PORT, "SHOW");
        json_object *data = json_tokener_parse(res);
        free(res);

        prettyPrint(data);

        return 0;
    }

    if (reboot) {
        char *res = udp_send(HOST, PORT, "REBOOT");
        (void)printf("%s\n", res);
        free(res);

        return 0;
    }

    if (on != 0) {
        char cmd[16];
        snprintf(cmd, 16, "SET PIN %d HIGH", on);

        char *res = udp_send(HOST, PORT, cmd);
        json_object *data = json_tokener_parse(res);
        free(res);

        prettyPrint(data);
    }

    if (off != 0) {
        char cmd[15];
        snprintf(cmd, 15, "SET PIN %d LOW", off);

        char *res = udp_send(HOST, PORT, cmd);
        json_object *data = json_tokener_parse(res);
        free(res);

        prettyPrint(data);
    }

    if (toggle != 0) {
        char cmd[19];
        snprintf(cmd, 19, "TOGGLE PIN %d HIGH", toggle);

        char *res = udp_send(HOST, PORT, cmd);
        json_object *data = json_tokener_parse(res);
        free(res);

        prettyPrint(data);
    }

    if (label != NULL) {
        if (pin == 0) {
            fprintf(stderr, "No pin specified! Please specify pin with the --pin option\n");
            return 1;
        }

        char cmd[278];
        snprintf(cmd, 278, "LABEL PIN %d %s", pin, label);

        char *res = udp_send(HOST, PORT, cmd);
        json_object *data = json_tokener_parse(res);
        free(res);

        prettyPrint(data);
    }

    return 0;
}