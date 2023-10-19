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

/*
typedef enum {
	OFF = 0,
	ON
} PinState;
*/
typedef struct {
	uint8_t state;
	char label[260];
} Pin;

Pin *get_pin(uint8_t i);

void write_to_file(void);
int get_port_state(void);

void* pin_to_mem(Pin *p, size_t* len);
// json_object* pin_to_json(Pin *p);

void load_parallel_from_file(void);

// const char* parallel_to_json(void);
void* parallel_to_mem(size_t *all_len);

char* read_file(FILE *fp, size_t* len);

#endif // PARALLEL_H
