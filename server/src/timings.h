#ifndef TIMINGS_H
#define TIMINGS_H

#include <stddef.h>

#include "parallel.h"

#define BroadcastCb(b) void (*b)(char *msg, size_t size)

typedef struct
{
    Parallel *port;
    BroadcastCb(broadcast);
} TimingArgs;

void start_timings_loop(TimingArgs *args);
void stop_timings_loop(void);

#endif // TIMINGS_H
