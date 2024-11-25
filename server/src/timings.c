#include "timings.h"
#include "config.h"
#include "parallel.h"
#include "globals.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

int needQuit(pthread_mutex_t *mtx)
{
    switch (pthread_mutex_trylock(mtx))
    {
    case 0: /* if we got the lock, unlock and return 1 (true) */
        pthread_mutex_unlock(mtx);
        return 1;
    case EBUSY: /* return 0 (false) if the mutex was locked */
        return 0;
    }
    return 1;
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
                bool isDay = FIRST_DAY(head->range) <= dt->tm_mday &&
                             LAST_DAY(head->range) >= dt->tm_mday;
                bool isMonth = FIRST_MONTH(head->range) <= dt->tm_mon &&
                               LAST_MONTH(head->range) >= dt->tm_mon;
                bool isHour = dt->tm_hour == head->hour;
                bool isMinute = dt->tm_min == head->minute;

                if (isMonth && isDay && isHour && isMinute && dt->tm_sec == 0)
                {
                    set_state(p, head->state);
                    config_dump(port);

                    char data[5120];
                    int ret = parallel_as_mem(port, data);
                    broadcast(data, ret);
                }

                head = head->next;
            }
        }

        sleep(1);
    }

    return NULL;
}