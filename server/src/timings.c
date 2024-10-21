#include "timings.h"
#include "config.h"
#include "parallel.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define PARA_LOOP(i) for (int i = 0; i < 8; i++)

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

    {
        time_t now = time(NULL);
        struct tm *dt = localtime(&now);
        sleep(60 - dt->tm_sec);
    }

    while (!needQuit(mtx))
    {
        PARA_LOOP(i)
        {
            time_t now = time(NULL);
            struct tm *dt = localtime(&now);

            Pin *p = port[i];
            Timing *head = p->timings;

            while (head != NULL)
            {
                bool isMonth = head->months & (1 << dt->tm_mon);
                bool isHour = dt->tm_hour == head->hour;
                bool isMinute = dt->tm_min == head->minute;

                if (isMonth && isHour && isMinute)
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

        sleep(60);
    }

    return NULL;
}