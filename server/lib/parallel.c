#include "parallel.h"
#include "globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/io.h>

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
        p->label = NULL;
        p->today_dirty = true;
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
    PARA_LOOP(i) { value |= (1 << i) * port[i]->state; }

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

        Timing *node = p->timings;
        while (node)
        {
            Timing *tmp = node->next;
            free(node);
            node = tmp;
        }

        if (p->today != NULL)
        {
            free(p->today);
        }
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
    Timing *clone = NULL;
    clone = malloc(sizeof(Timing));

    if (clone == NULL)
        return;

    memcpy(clone, timing, sizeof(Timing));

    clone->next = pin->timings;
    pin->timings = clone;

    pin->today_dirty = true;
}

void remove_timing(Pin *pin, Timing *timing)
{
    Timing *prev = NULL;
    Timing *head = pin->timings;

    while (head != NULL && !compareTiming(head, timing))
    {
        prev = head;
        head = head->next;
    }

    if (head == NULL)
        return;

    if (prev == NULL)
        pin->timings = head->next;
    else
        prev->next = head->next;

    pin->today_count = 0;
    pin->today_dirty = true;

    free(head);
}

void remove_timings(Pin *pin)
{
    pin->today_count = 0;
    pin->today_dirty = true;

    Timing *head = pin->timings;

    while (head != NULL)
    {
        Timing *to_free = head;
        head = to_free->next;
        free(to_free);
    }

    pin->timings = NULL;
}

static bool date_in_range(uint8_t mon, uint8_t day,
                          uint8_t f_mon, uint8_t f_day,
                          uint8_t l_mon, uint8_t l_day)
{
    int t = mon * 32 + day;
    int f = f_mon * 32 + f_day;
    int l = l_mon * 32 + l_day;
    return t >= f && t <= l;
}

static int cmp_timing_key(const void *a, const void *b)
{
    const Timing *ta = *(const Timing *const *)a;
    const Timing *tb = *(const Timing *const *)b;
    return (ta->hour * 60 + ta->minute) - (tb->hour * 60 + tb->minute);
}

void build_index(Pin *pin, const struct tm *dt)
{
    uint8_t mon = dt->tm_mon + 1;
    uint8_t day = dt->tm_mday;

    uint16_t count = 0;
    for (Timing *t = pin->timings; t != NULL; t = t->next)
    {
        if (date_in_range(mon, day, t->range.first_month, t->range.first_day,
                          t->range.last_month, t->range.last_day))
            count++;
    }

    if (count > pin->today_cap)
    {
        Timing **buf = realloc(pin->today, count * sizeof(Timing *));
        if (buf == NULL)
        {
            pin->today_count = 0;
            pin->today_dirty = false;
            return;
        }
        pin->today = buf;
        pin->today_cap = count;
    }

    uint16_t idx = 0;
    for (Timing *t = pin->timings; t != NULL; t = t->next)
    {
        if (date_in_range(mon, day, t->range.first_month, t->range.first_day,
                          t->range.last_month, t->range.last_day))
            pin->today[idx++] = t;
    }

    pin->today_count = count;

    if (count > 1)
        qsort(pin->today, count, sizeof(Timing *), cmp_timing_key);

    pin->today_dirty = false;
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

    Timing *head = NULL;
    head = p->timings;
    while (head)
    {
        *buf++ = head->range.first_day;
        *buf++ = head->range.first_month;
        *buf++ = head->range.last_day;
        *buf++ = head->range.last_month;

        *buf++ = head->hour;
        *buf++ = head->minute;
        *buf++ = head->state;

        head = head->next;
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
