#include "command.h"
#include "log.h"
#include "parallel.h"
#include "timings.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    ws_sendframe_bin_bcast(5663, (const char *)data, len);
}

size_t onBaseRequest(const char *req, size_t req_len, char **res)
{
    static const char notFound[] =
        "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
    static const char ok[] = "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n";

    char *p = strchr(req, ' ') + 1;
    size_t len = strchr(p, ' ') - p - 1;

    if (len != 10)
    {
        *res = malloc(sizeof(notFound) - 1);
        memcpy(*res, notFound, sizeof(notFound) - 1);
        return sizeof(notFound) - 1;
    }

    if (memcmp(p, "/toggle?p=", len - 1) != 0)
    {
        *res = malloc(sizeof(notFound) - 1);
        memcpy(*res, notFound, sizeof(notFound) - 1);
        return sizeof(notFound) - 1;
    }

    int pin = *(p + 10) - '0';

    AST ast;
    char *errMsg;

    uint8_t cmd[] = { Toggle, pin };
    int ret = command_parse((void *)cmd, 2, &ast, &errMsg);

    if (!ret)
    {
        log_error("%s", errMsg);
        *res = malloc(sizeof(notFound) - 1);
        memcpy(*res, notFound, sizeof(notFound) - 1);
        return sizeof(notFound) - 1;
    }

    uint8_t data[5120];
    size_t resLen = command_exec(&ast, port, data, &errMsg);

    if (resLen < 1)
    {
        *res = malloc(sizeof(notFound) - 1);
        memcpy(*res, notFound, sizeof(notFound) - 1);
        return sizeof(notFound) - 1;
    }

    ws_sendframe_bin_bcast(5663, (const char *)data, len);

    *res = malloc(sizeof(ok) - 1);
    memcpy(*res, ok, sizeof(ok) - 1);
    return sizeof(ok) - 1;
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
        .path = "/",
        .host = "0.0.0.0",
        .port = 5663,
        .thread_loop = 0,
        .timeout_ms = 1000,
        .evs.onmessage = &onMessage,
        .evs.onopen = &onOpenClose,
        .evs.onclose = &onOpenClose,
        .evs.onbaserequest = &onBaseRequest,
    });

    pthread_mutex_unlock(&mtx);
    pthread_join(th1, NULL);

    free_parallel(port);

    return 0;
}
