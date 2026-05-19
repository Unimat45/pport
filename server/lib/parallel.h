#ifndef PARALLEL_H
#define PARALLEL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include "pport_export.h"

#define ON 1
#define OFF 0

#define MAX_LABEL 64
#define TIMING_LEN sizeof(Timing)

// 8 pins with label and state + 50 timings allocated each + version +
// enabled_pins
#define MAX_PORT_SIZE (8 * (MAX_LABEL + 1 + TIMING_LEN * 50) + 2)

typedef struct Timing
{
    union
    {
        struct
        {
            uint8_t last_month;
            uint8_t last_day;
            uint8_t first_month;
            uint8_t first_day;
        };
        uint32_t mixed;
    } range;
    uint8_t hour;
    uint8_t minute;
    uint8_t state;
    struct Timing *next;
} Timing;

typedef struct Pin
{
    uint8_t state;
    char *label;
    Timing *timings;

    Timing **today;
    uint16_t today_count;
    uint16_t today_cap;
    bool today_dirty;
} Pin;

typedef struct Parallel
{
    Pin *pins[8];
    uint8_t enabled_pins;
} Parallel;

PPORT_EXPORT Parallel *init_port(void);
PPORT_EXPORT void free_parallel(Parallel *port);

PPORT_EXPORT void set_state(Pin *pin, uint8_t state);
PPORT_EXPORT size_t set_label(Pin *pin, const char *label);
PPORT_EXPORT void add_timing(Pin *pin, Timing *timing);
PPORT_EXPORT void remove_timing(Pin *pin, Timing *timing);
PPORT_EXPORT void remove_timings(Pin *pin);
PPORT_EXPORT void build_index(Pin *pin, const struct tm *dt);

PPORT_EXPORT size_t parallel_as_mem(Parallel *port, void *restrict data);

#endif // !PARALLEL_H
