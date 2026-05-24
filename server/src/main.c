#include "command.h"
#include "config.h"
#include "log.h"
#include "parallel.h"
#include "timings.h"

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ws.h>

static volatile sig_atomic_t running = 1;

static void handle_signal(int sig)
{
    (void)sig;
    running = 0;
}

Parallel *port = NULL;

static void onOpenClose(ws_cli_conn_t client) { (void)client; }

static void broadcastCb(char *data, size_t size)
{
    config_dump(port);
    ws_sendframe_bin_bcast(5663, (const char *)data, size);
}

void onMessage(ws_cli_conn_t client, const uint8_t *cmd, uint64_t size,
               int type)
{
    AST ast;
    char *errMsg;

    int ret = command_parse((void *)cmd, size, &ast, &errMsg);

    if (!ret)
    {
        ws_sendframe_txt(client, errMsg);
        log_error("%s", errMsg);
        return;
    }

    uint8_t data[MAX_PORT_SIZE];
    size_t len = command_exec(&ast, port, data, &errMsg);

    if (len < 1)
    {
        ws_sendframe_txt(client, errMsg);
        log_error("%s: %d", errMsg, *cmd);
    }
    else if (ast.action == NextTrigger)
    {
        Timing t;
        memcpy(&t, data, sizeof(Timing));
        char buf[128];
        if (t.hour == 0xFF)
            snprintf(buf, sizeof(buf), "{\"nextTrigger\":null}");
        else
            snprintf(buf, sizeof(buf),
                     "{\"nextTrigger\":{\"hour\":%d,\"min\":%d,\"state\":%s}}",
                     t.hour, t.minute, t.state ? "true" : "false");
        ws_sendframe_txt(client, buf);
    }
    else
    {
        ws_sendframe_bin_bcast(5663, (const char *)data, len);
    }

    switch (ast.action)
    {
    case Set:
    case Label:
    case Toggle:
    case Timings:
    case DeleteTiming:
    case DeleteAllTimings:
        config_dump(port);
        break;
    case Show:
    case NotSet:
    default:
        break;
    }
}

int main(void)
{
    port = init_port();

    if (port == NULL)
    {
        fprintf(stderr, "Error initializing parallel port\n");
        return 1;
    }

    config_load(port);

    TimingArgs args = {port, &broadcastCb};
    start_timings_loop(&args);

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    ws_socket(&(struct ws_server){
        .path = "/",
        .host = "0.0.0.0",
        .port = 5663,
        .thread_loop = 1,
        .timeout_ms = 1000,
        .evs.onmessage = &onMessage,
        .evs.onopen = &onOpenClose,
        .evs.onclose = &onOpenClose,
    });

    while (running)
    {
        sleep(1);
    }

    log_info("Cleaning up...");
    ws_shutdown();
    stop_timings_loop();

    free_parallel(port);

    return 0;
}
