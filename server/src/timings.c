#ifdef LOG
#include "log.h"
#endif

#include "config.h"
#include "globals.h"
#include "parallel.h"
#include "timings.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

int needQuit(pthread_mutex_t *mtx)
{
    switch (pthread_mutex_trylock(mtx))
    {
    case 0:
        /* if we got the lock, loop shall stop */
        pthread_mutex_unlock(mtx);
        return 1;
    case EBUSY:
        /* if locked, loop shall continue */
        return 0;
    }
    return 1;
}

int8_t compareDates(struct tm *a, struct tm *b)
{
    int diff = 0;

    diff = b->tm_year - a->tm_year;
    if (diff != 0)
        goto end;

    diff = b->tm_mon - a->tm_mon;
    if (diff != 0)
        goto end;

    diff = b->tm_mday - a->tm_mday;
    if (diff != 0)
        goto end;

    diff = b->tm_hour - a->tm_hour;
    if (diff != 0)
        goto end;

    diff = b->tm_min - a->tm_min;
    if (diff != 0)
        goto end;

    diff = b->tm_sec - a->tm_sec;

end:
    return diff;
}

void *timings_loop(void *ptr)
{
    Parallel *port = NULL;
    TimingArgs *args = NULL;
    BroadcastCb(broadcast) = NULL;
    pthread_mutex_t *mtx = NULL;

    args = (TimingArgs *)ptr;
    port = args->port;
    mtx = args->mtx;
    broadcast = args->broadcast;

    while (!needQuit(mtx))
    {
        PARA_LOOP(i)
        {
            time_t now = time(NULL);
            struct tm *dt = localtime(&now);

            // Change months range to 1-12
            dt->tm_mon++;

            Pin *p = port[i];
            Timing *head = p->timings;

            while (head != NULL)
            {
                struct tm first = {0,
                                   head->minute,
                                   head->hour,
                                   head->range.first_day,
                                   head->range.first_month,
                                   dt->tm_year,
                                   0,
                                   0,
                                   0,
                                   0,
                                   NULL};
                struct tm last = {0,
                                  head->minute,
                                  head->hour,
                                  head->range.last_day,
                                  head->range.last_month,
                                  dt->tm_year,
                                  0,
                                  0,
                                  0,
                                  0,
                                  NULL};

                if (compareDates(dt, &first) <= 0 &&
                    compareDates(dt, &last) >= 0 && dt->tm_sec == 0)
                {
                    set_state(p, head->state);
                    config_dump(port);

                    char data[5120];
                    int ret = parallel_as_mem(port, data);
                    broadcast(data, ret);

#ifdef LOG
                    log_info("TIMER HIT FOR PIN %d %d %s - %d %s %02d:%02d %s",
                             i, head->range.first_day,
                             months[head->range.first_month - 1],
                             head->range.last_day,
                             months[head->range.last_month - 1], head->hour,
                             head->minute, head->state ? "ON" : "OFF");

#endif
                }

                head = head->next;
            }
        }

        sleep(1);
    }

    return NULL;
}