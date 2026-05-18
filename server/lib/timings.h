#ifndef TIMINGS_H
#define TIMINGS_H

#include <stdint.h>
#include <stddef.h>

#include "parallel.h"
#include "pport_export.h"

#define BroadcastCb(b) void (*b)(char *msg, size_t size)

typedef struct
{
    Parallel *port;
    BroadcastCb(broadcast);
} TimingArgs;

PPORT_EXPORT void start_timings_loop(TimingArgs *args);
PPORT_EXPORT void stop_timings_loop(void);

#endif // TIMINGS_H