#ifdef LOG
#include "log.h"
#endif

#include "globals.h"
#include "timings.h"

#include <asm-generic/errno-base.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

pthread_t th;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

int needQuit(void)
{
    switch (pthread_mutex_trylock(&mtx))
    {
    case 0:
        pthread_mutex_unlock(&mtx);
        return 1;
    case EBUSY:
        return 0;
    }
    return 1;
}

void *timings_loop(void *ptr)
{
    Parallel *port = NULL;
    TimingArgs *args = NULL;
    BroadcastCb(broadcast) = NULL;

    args = (TimingArgs *)ptr;
    port = args->port;
    broadcast = args->broadcast;

    while (!needQuit())
    {
        time_t now_epoch = time(NULL);
        struct tm *dt = localtime(&now_epoch);

        if (dt->tm_sec != 0)
        {
            sleep(1);
            continue;
        }

        int key = dt->tm_hour * 60 + dt->tm_min;
        uint8_t mon = dt->tm_mon + 1;
        uint8_t day = dt->tm_mday;

        PARA_LOOP(i)
        {
            Pin *p = port->pins[i];

            int lo = 0, hi = (int)p->timings_count - 1;
            int found = -1;
            while (lo <= hi)
            {
                int mid = lo + (hi - lo) / 2;
                int mk = p->timings[mid]->hour * 60 + p->timings[mid]->minute;
                if (mk < key)
                    lo = mid + 1;
                else if (mk > key)
                    hi = mid - 1;
                else
                {
                    found = mid;
                    break;
                }
            }

            if (found < 0)
                continue;

            int start = found;
            while (start > 0 && p->timings[start - 1]->hour * 60 +
                                        p->timings[start - 1]->minute ==
                                    key)
                start--;

            int end = found;
            while (end < (int)p->timings_count - 1 &&
                   p->timings[end + 1]->hour * 60 +
                           p->timings[end + 1]->minute ==
                       key)
                end++;

            for (int j = start; j <= end; j++)
            {
                Timing *t = p->timings[j];

                if (!date_in_range(mon, day, t->range.first_month,
                                   t->range.first_day, t->range.last_month,
                                   t->range.last_day))
                    continue;

                set_state(p, t->state);

                char data[MAX_PORT_SIZE];
                int ret = parallel_as_mem(port, data);
                broadcast(data, ret);

#ifdef LOG
                log_info("TIMER HIT FOR PIN %d %d %s - %d %s %02d:%02d %s",
                         i + 2, t->range.first_day,
                         months[t->range.first_month - 1], t->range.last_day,
                         months[t->range.last_month - 1], t->hour, t->minute,
                         t->state ? "ON" : "OFF");
#endif
            }
        }

        sleep(1);
    }

    return NULL;
}

void start_timings_loop(TimingArgs *args)
{
    pthread_mutex_init(&mtx, NULL);
    pthread_mutex_lock(&mtx);

    (void)pthread_create(&th, NULL, &timings_loop, args);
}

void stop_timings_loop(void)
{
    pthread_mutex_unlock(&mtx);
    pthread_join(th, NULL);
}
