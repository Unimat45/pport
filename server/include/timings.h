#ifndef TIMINGS_H
#define TIMINGS_H

#include <stdint.h>
#include <pthread.h>

#define BroadcastCb(b) void (*b)(char *msg, size_t size)
typedef struct Pin *Parallel;

typedef struct
{
    pthread_mutex_t *mtx;
    Parallel *port;
    BroadcastCb(broadcast);
} TimingArgs;

void *timings_loop(void *ptr);

#endif // TIMINGS_H