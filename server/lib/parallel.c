#include "parallel.h"
#include "globals.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/io.h>
#include <time.h>

bool date_in_range(uint8_t mon, uint8_t day, uint8_t f_mon, uint8_t f_day,
                   uint8_t l_mon, uint8_t l_day)
{
    int t = mon * 32 + day;
    int f = f_mon * 32 + f_day;
    int l = l_mon * 32 + l_day;
    return t >= f && t <= l;
}

int compareTiming(Timing *a, Timing *b)
{
    return a->range.mixed == b->range.mixed && a->hour == b->hour &&
           a->minute == b->minute && a->state == b->state;
}

Parallel *init_port(void)
{
#ifdef NDEBUG
    if (ioperm(PPORT, 1, 1))
    {
        return NULL;
    }
#endif

    int free_i;
    Parallel *port = NULL;

    port = calloc(1, sizeof(Parallel));

    if (port == NULL)
    {
        return NULL;
    }

    port->enabled_pins = 0xFF;

    PARA_LOOP(i)
    {
        Pin *p = NULL;
        p = calloc(1, sizeof(Pin));

        if (p == NULL)
        {
            free_i = i - 1;
            goto err_free_para;
        }

        p->state = 0;
        p->timings = NULL;
        p->timings_count = 0;
        p->timings_cap = 0;
        p->label = calloc(1, 6);

        if (p->label == NULL)
        {
            free_i = i - 1;
            goto err_free_para;
        }

        snprintf((char *)p->label, 6, "Pin %d", i + 2);

        port->pins[i] = p;
    }

#ifdef NDEBUG
    uint8_t value = 0;
    PARA_LOOP(i) { value |= (1 << i) * port->pins[i]->state; }

    outb(value, PPORT);
#endif

    return port;

err_free_para:
    for (int i = free_i; i >= 0; i--)
    {
        Pin *p = port->pins[i];

        if (p)
        {
            if (p->label)
            {
                free((void *)p->label);
            }

            free(p);
        }
    }

    free(port);
    return NULL;
}

void free_parallel(Parallel *port)
{
    PARA_LOOP(i)
    {
        Pin *p = port->pins[i];

        if (p->label)
        {
            free((void *)p->label);
        }

        for (size_t i = 0; i < p->timings_count; i++)
        {
            free(p->timings[i]);
        }

        free(p->timings);
        free(p);
    }

    free(port);
}

void set_state(Pin *pin, uint8_t state) { pin->state = state; }

size_t set_label(Pin *pin, const char *label)
{
    char *old = NULL;

    if (pin->label != NULL)
    {
        old = pin->label;
    }

    size_t len = strnlen(label, MAX_LABEL);

    pin->label = malloc(len + 1);

    if (pin->label == NULL)
    {
        pin->label = old;
        return 0;
    }

    if (old != NULL)
    {
        free((void *)old);
    }

    memcpy((void *)pin->label, label, len);
    // String terminator
    memset((void *)(pin->label + len), 0, 1);

    return len;
}

void add_timing(Pin *pin, Timing *timing)
{
    if (pin->timings_count >= pin->timings_cap)
    {
        size_t new_cap = pin->timings_cap ? pin->timings_cap * 2 : 4;
        Timing **buf = realloc(pin->timings, new_cap * sizeof(Timing *));
        if (buf == NULL)
            return;
        pin->timings = buf;
        pin->timings_cap = new_cap;
    }

    Timing *clone = malloc(sizeof(Timing));
    if (clone == NULL)
        return;

    memcpy(clone, timing, sizeof(Timing));

    int key = clone->hour * 60 + clone->minute;
    size_t pos = pin->timings_count;
    while (pos > 0 &&
           pin->timings[pos - 1]->hour * 60 + pin->timings[pos - 1]->minute >
               key)
        pos--;

    memmove(&pin->timings[pos + 1], &pin->timings[pos],
            (pin->timings_count - pos) * sizeof(Timing *));
    pin->timings[pos] = clone;
    pin->timings_count++;
}

void remove_timing(Pin *pin, Timing *timing)
{
    for (size_t i = 0; i < pin->timings_count; i++)
    {
        if (compareTiming(pin->timings[i], timing))
        {
            free(pin->timings[i]);
            pin->timings_count--;
            memmove(&pin->timings[i], &pin->timings[i + 1],
                    (pin->timings_count - i) * sizeof(Timing *));
            return;
        }
    }
}

void remove_all_timings(Pin *pin)
{
    for (size_t i = 0; i < pin->timings_count; i++)
    {
        free(pin->timings[i]);
    }

    free(pin->timings);

    pin->timings = NULL;
    pin->timings_count = 0;
    pin->timings_cap = 0;
}

size_t next_timing(Pin *pin)
{
    if (pin->timings_count == 0)
        return 0;

    time_t now_epoch = time(NULL);
    struct tm *dt = localtime(&now_epoch);

    int dt_key = dt->tm_hour * 60 + dt->tm_min;
    uint8_t mon = dt->tm_mon + 1;
    uint8_t day = dt->tm_mday;

    size_t low = 0, high = pin->timings_count;
    while (low < high)
    {
        size_t mid = low + (high - low) / 2;
        int key = pin->timings[mid]->hour * 60 + pin->timings[mid]->minute;
        if (key < dt_key)
            low = mid + 1;
        else
            high = mid;
    }

    for (size_t i = low; i < pin->timings_count; i++)
    {
        Timing *t = pin->timings[i];
        if (date_in_range(mon, day, t->range.first_month, t->range.first_day,
                          t->range.last_month, t->range.last_day))
            return i;
    }

    return pin->timings_count;
}

size_t serialize_pin(Pin *p, void *restrict data)
{
    if (data == NULL)
        return 0;
    uint8_t *buf = (uint8_t *)data;

    size_t lbl_len = strnlen(p->label, MAX_LABEL);
    *buf++ = p->state;

    memcpy(buf, p->label, lbl_len);

    buf += lbl_len;
    // String terminator
    *buf++ = 0;

    for (size_t i = 0; i < p->timings_count; i++)
    {
        Timing *t = p->timings[i];
        *buf++ = t->range.first_day;
        *buf++ = t->range.first_month;
        *buf++ = t->range.last_day;
        *buf++ = t->range.last_month;

        *buf++ = t->hour;
        *buf++ = t->minute;
        *buf++ = t->state;
    }
    *buf++ = 0;
    *buf++ = 0;

    return buf - (uint8_t *)data;
}

size_t parallel_as_mem(Parallel *port, void *restrict data)
{
    uint8_t *base = data;
    size_t len;

    *base++ = CFG_VERSION;
    *base++ = port->enabled_pins;

    PARA_LOOP(i)
    {
        Pin *p = port->pins[i];

        len = serialize_pin(p, base);
        base += len;
    }

    return base - (uint8_t *)data;
}
