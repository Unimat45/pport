#ifndef PARALLEL_H
#define PARALLEL_H

#include <stdio.h>
#include "json-c/json.h"

#define PORT 0x378

#ifdef _MSC_VER
#define STATE_FILE "pport.conf"
#else
#define STATE_FILE "/etc/pport.conf"
#endif

typedef enum {
	OFF = 0,
	ON
} PinState;

typedef struct {
	char label[260];
	PinState state;
} Pin;

Pin *get_pin(uint8_t i);
void write_to_file(void);
int get_port_state(void);
json_object* pin_to_json(Pin *p);
void load_parallel_from_file(void);
const char* parallel_to_json(void);
char* read_file(FILE *fp, size_t* len);

#endif // PARALLEL_H