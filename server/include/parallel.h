#ifndef PARALLEL_H
#define PARALLEL_H

#include <stdint.h>
#include <stdio.h>
#include "json-c/json.h"

#define PORT 0x378

#ifdef _MSC_VER
#define STATE_FILE "pport.conf"
#else
#define STATE_FILE "/etc/pport.conf"
#endif

#define OFF 0
#define ON 1

typedef struct {
	uint8_t state;
	char label[260];
} Pin;

void write_to_file(Pin parallel[8]);
void load_parallel_from_file(Pin parallel[8]);

size_t pin_to_mem(void *buf, Pin *p);
size_t parallel_to_mem(void *buf, Pin parallel[8]);

#endif // PARALLEL_H
