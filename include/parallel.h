#ifndef PARALLEL_H
#define PARALLEL_H

#define PORT 0x378
#define STATE_FILE "/etc/pport.state"

typedef enum {
	OFF = 0,
	ON
} PinState;

typedef struct {
	char* label;
	PinState state;
} Pin;

Pin parallel[8];

void load_parallel_from_file();
void write_to_file();

int get_port_state();

#endif // PARALLEL_H