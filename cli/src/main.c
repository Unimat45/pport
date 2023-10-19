#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <stdlib.h>

#include "argparse.h"
#include "udp_send.h"

#define HOST "127.0.0.1"
#define PORT 5663

typedef struct {
	uint8_t state;
	char label[260];
} Pin;

static const char *const usages[] = {
    "pport [options]",
    NULL,
};

int findLongestLabel(Pin p[8]) {
    int longest = 0;

    for (size_t i = 0; i < 8; i++) {
        int l = (int)strlen(p[i].label);

        if (l > longest) {
            longest = l;
        }
    }

    return longest;
}

void prettyPrint(void *data) {
  uint8_t is_array = *(uint8_t*)data;

  if (is_array) {
    Pin parallel[8];

    size_t last_len = 1;
    for (size_t i = 0; i < 8; i++) {
      Pin p;

      memcpy(&p, data + last_len, 1);
      strncpy(p.label, data + last_len + 1, 259);

      last_len += strlen(p.label) + 2; // 2 for state + terminator
      parallel[i] = p;
    }

    int longest = findLongestLabel(parallel);
    
    for (size_t i = 0; i < 8; i++) {
      Pin p = parallel[i];

      int padding = longest - (int)strlen(p.label);

      if (!padding) {
        printf("%s: %s\n", p.label, p.state ? "On" : "Off");
      }
      else {
        char buf[260];
        memset(buf, ' ', padding);
        buf[padding] = 0;

        printf("%s%s: %s\n", p.label, buf, p.state ? "On" : "Off");
      }
    }
  }
  else {
    Pin p;
    memcpy(&p, data + 1, 1);
    strncpy(p.label, data + 2, 259);
    printf("%s: %s\n", p.label, p.state ? "On" : "Off");
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

            size_t res_len;
            void *res = udp_send(HOST, PORT, cmd, &res_len);

            if (strstr(res, "STOP") != NULL) {
              free(res);
              return 0;
            }

            if (strstr(res, "ERROR") != NULL) {
                (void)printf("%s\n", (char*)res);
            }
            else {
                prettyPrint(res);
            }

            free(res);
        }
    }

    struct argparse argparse;
    (void)argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\nThis program is used to control parallel port data pins on a UNIX machine", "\nAvailable pin numbers are: 2, 3, 4, 5, 6, 7, 8, 9");
    argc = argparse_parse(&argparse, argc, (const char **)argv);

    if (status) {
        char *res = udp_send(HOST, PORT, "SHOW", NULL);
        
        prettyPrint(res);

        free(res);

        return 0;
    }

    if (reboot) {
        char *res = udp_send(HOST, PORT, "REBOOT", NULL);
        (void)printf("%s\n", res);
        free(res);

        return 0;
    }

    if (on != 0) {
        char cmd[16];
        snprintf(cmd, 16, "SET PIN %d HIGH", on);

        char *res = udp_send(HOST, PORT, cmd, NULL);
        prettyPrint(res);
        free(res);
    }

    if (off != 0) {
        char cmd[15];
        snprintf(cmd, 15, "SET PIN %d LOW", off);

        char *res = udp_send(HOST, PORT, cmd, NULL);
        prettyPrint(res);
        free(res);
    }

    if (toggle != 0) {
        char cmd[19];
        snprintf(cmd, 19, "TOGGLE PIN %d HIGH", toggle);

        char *res = udp_send(HOST, PORT, cmd, NULL);
        prettyPrint(res);
        free(res);
    }

    if (label != NULL) {
        if (pin == 0) {
            fprintf(stderr, "No pin specified! Please specify pin with the --pin option\n");
            return 1;
        }

        char cmd[278];
        snprintf(cmd, 278, "LABEL PIN %d %s", pin, label);

        char *res = udp_send(HOST, PORT, cmd, NULL);
        prettyPrint(res);
        free(res);
    }

    return 0;
}
