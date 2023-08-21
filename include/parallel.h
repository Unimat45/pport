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
	char* label;
	PinState state;
} Pin;

Pin *parallel[8];

char* read_file(FILE *fp, size_t* len);
void load_parallel_from_file(void);
void write_to_file(void);
int get_port_state(void);
json_object* pin_to_json(Pin *p);
char* parallel_to_json(void);

#endif // PARALLEL_H