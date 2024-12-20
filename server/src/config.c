#include "config.h"
#include "globals.h"
#include "parallel.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/io.h>

#ifndef CFG_FILE
#define CFG_FILE "pport.cfg"
#endif

int config_load(Parallel *port)
{
    FILE *fd = NULL;
    fd = fopen(CFG_FILE, "rb");

    if (fd == NULL)
    {
        return 0;
    }

    fseek(fd, 0, SEEK_END);
    size_t size = ftell(fd);
    rewind(fd);

    uint8_t *buf = NULL, *start = NULL;
    buf = malloc(size);
    start = buf;

    if (buf == NULL)
    {
        fclose(fd);
        return 0;
    }

    size = fread(buf, 1, size, fd);
    fclose(fd);

    PARA_LOOP(i)
    {
        Pin *p = port[i];

        p->state = *buf++;

        size_t lbl_len = set_label(p, (const char *)buf);
        // +1 for string terminator
        buf += lbl_len + 1;

        while (*buf != 0 || *(buf + 1) != 0)
        {
            Timing t;
            t.range.first_day = *buf++;
            t.range.first_month = *buf++;
            t.range.last_day = *buf++;
            t.range.last_month = *buf++;

            t.hour = *buf++;
            t.minute = *buf++;
            t.state = *buf++;
            t.next = NULL;

            add_timing(p, &t);
        }
        buf += 2;
    }

    free(start);

    return 1;
}

uint8_t calculate_value(Parallel *port)
{
    uint8_t value = 0;
    PARA_LOOP(i) { value |= (1 << i) * port[i]->state; }

    return value;
}

void config_dump(Parallel *port)
{
    uint8_t data[5120];
    size_t size = parallel_as_mem(port, data);

    FILE *fd = NULL;
    fd = fopen(CFG_FILE, "wb");

    if (fd == NULL)
    {
        return;
    }

    fwrite(data, 1, size, fd);

    fclose(fd);

    uint8_t value = calculate_value(port);

#ifdef NDEBUG
    outb(value, PPORT);
#endif
}
