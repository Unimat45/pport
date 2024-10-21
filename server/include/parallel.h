#ifndef PARALLEL_H
#define PARALLEL_H

#include <stdint.h>
#include <stddef.h>

#define ON 1
#define OFF 0

#define MAX_LABEL 64
#define TIMING_LEN 5

typedef struct Timing {
    uint16_t months;
    uint8_t hour;
    uint8_t minute;
    uint8_t state;
    struct Timing *next;
} Timing;

typedef struct Pin {
    uint8_t state;
    const char *label;
    Timing *timings;
} Pin, *Parallel;

Parallel *init_port(void);
void free_parallel(Parallel *port);

void set_state(Pin *pin, uint8_t state);
size_t set_label(Pin *pin, const char *label);
void add_timing(Pin *pin, Timing *timing);
void remove_timing(Pin *pin, Timing *timing);
void remove_timings(Pin *pin);

void set_all_state(Parallel *port, uint8_t state);

size_t parallel_as_mem(Parallel *port, void *restrict data);

#endif // !PARALLEL_H
