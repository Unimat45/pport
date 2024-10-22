#include "parallel.h"
#include "config.h"
#include "globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/io.h>

#define PARA_LOOP(i) for (int i = 0; i < 8; i++)

int compareTiming(Timing *a, Timing *b)
{
    return a->months == b->months && a->hour == b->hour &&
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

    port = malloc(sizeof(Parallel) * 8);

    if (port == NULL)
    {
        return NULL;
    }

    PARA_LOOP(i)
    {
        Pin *p = NULL;
        p = malloc(sizeof(Pin));

        if (p == NULL)
        {
            free_i = i - 1;
            goto err_free_para;
        }

        p->state = 0;
        p->timings = NULL;
        p->label = NULL;
        p->label = malloc(6);

        if (p->label == NULL)
        {
            free_i = i - 1;
            goto err_free_para;
        }

        memset((void *)p->label, 0, 6);
        snprintf((char *)p->label, 6, "Pin %d", i + 2);

        port[i] = p;
    }

    config_load(port);

    uint8_t value = 0;
    PARA_LOOP(i) { value |= (1 << i) * port[i]->state; }

#ifdef NDEBUG
    outb(value, PPORT);
#endif

    return port;

err_free_para:
    for (int i = free_i; i >= 0; i--)
    {
        Pin *p = port[i];

        if (p)
        {
            if (p->label)
            {
                free((void *)p->label);
            }

            free(p);
        }
    }

    return NULL;
}

void free_parallel(Parallel *port)
{
    PARA_LOOP(i)
    {
        Pin *p = port[i];

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

        free(p);
    }

    free(port);
}

void set_state(Pin *pin, uint8_t state) { pin->state = state; }

size_t set_label(Pin *pin, const char *label)
{
    const char *old = NULL;

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
}

void remove_timing(Pin *pin, Timing *timing)
{
    Timing *head = pin->timings;

    while (head != NULL && !compareTiming(head->next, timing))
    {
        head = head->next;
    }

    Timing *to_free = head->next;
    head->next = to_free->next;

    free(to_free);
}

void remove_timings(Pin *pin)
{
    Timing *head = pin->timings;

    while (head != NULL)
    {
        Timing *to_free = head;

        head = to_free->next;

        free(to_free);
    }

    pin->timings = NULL;
}

void set_all_state(Parallel *port, uint8_t state)
{
    PARA_LOOP(i) { port[i]->state = state; }
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
        *buf++ = (head->months >> 8) & 0xFF;
        *buf++ = head->months & 0xFF;

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

    PARA_LOOP(i)
    {
        Pin *p = port[i];

        len = serialize_pin(p, base);
        base += len;
    }

    return base - (uint8_t *)data;
}
