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
#include <string.h>

pthread_t th;
pthread_mutex_t mtx;

int needQuit(void)
{
    switch (pthread_mutex_trylock(&mtx))
    {
    case 0:
        /* if we got the lock, loop shall stop */
        pthread_mutex_unlock(&mtx);
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

end:
    return diff;
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
        PARA_LOOP(i)
        {
            time_t now_epoch = time(NULL);
            struct tm *dt = localtime(&now_epoch);

            // Change months range to 1-12
            dt->tm_mon++;

            Pin *p = port->pins[i];
            Timing *head = p->timings;

            while (head != NULL)
            {
                struct tm first, last;
                memset(&first, 0, sizeof(first));
                memset(&last, 0, sizeof(last));

                first.tm_mday = head->range.first_day;
                first.tm_mon = head->range.first_month;
                first.tm_year = dt->tm_year;

                last.tm_mday = head->range.last_day;
                last.tm_mon = head->range.last_month;
                last.tm_year = dt->tm_year;

                bool isDay = compareDates(dt, &first) <= 0 &&
                             compareDates(dt, &last) >= 0;

                bool isHour = dt->tm_hour == head->hour;
                bool isMinute = dt->tm_min == head->minute;

                if (isDay && isHour && isMinute && dt->tm_sec == 0)
                {
                    set_state(p, head->state);

                    char data[MAX_PORT_SIZE];
                    int ret = parallel_as_mem(port, data);
                    broadcast(data, ret);

#ifdef LOG
                    log_info("TIMER HIT FOR PIN %d %d %s - %d %s %02d:%02d %s",
                             i + 2, head->range.first_day,
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