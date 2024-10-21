#include "command.h"
#include "log.h"
#include "parallel.h"
#include "timings.h"
#include <pthread.h>
#include <stdio.h>
#include <ws.h>

Parallel *port = NULL;

static void onOpenClose(ws_cli_conn_t client) { (void)client; }

static void broadcastCb(char *data, size_t size)
{
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

    uint8_t data[5120];
    size_t len = command_exec(&ast, port, data, &errMsg);

    if (len < 1)
    {
        ws_sendframe_txt(client, errMsg);
        log_error("%s: %d", errMsg, *cmd);
        return;
    }

    // log_info("%s", ws_getaddress(client));
    ws_sendframe_bin_bcast(5663, (const char *)data, len);
}

int main(void)
{
    port = init_port();

    if (port == NULL)
    {
        fprintf(stderr, "Error initializing parallel port\n");
        return 1;
    }

    pthread_t th1;
    pthread_mutex_t mtx;

    pthread_mutex_init(&mtx, NULL);
    pthread_mutex_lock(&mtx);

    TimingArgs args = {&mtx, port, &broadcastCb};
    (void)pthread_create(&th1, NULL, &timings_loop, &args);

    ws_socket(&(struct ws_server){
        .host = "0.0.0.0",
        .port = 5663,
        .thread_loop = 0,
        .timeout_ms = 1000,
        .evs.onmessage = &onMessage,
        .evs.onopen = &onOpenClose,
        .evs.onclose = &onOpenClose,
    });

    pthread_mutex_unlock(&mtx);
    pthread_join(th1, NULL);

    free_parallel(port);

    return 0;
}
