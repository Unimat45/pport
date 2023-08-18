#include "parallel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json-c/json_object.h"
#include "json-c/json_tokener.h"

#define DEFAULT_LABEL "Pin "

static void free_parallel_labels(void) {
    for (size_t i = 0; i < sizeof(parallel); i++) {
		if (parallel[i].label) {
        	free(parallel[i].label);
		}
    }
}

void load_parallel_from_file() {
	atexit(free_parallel_labels);

	FILE *fp = fopen(STATE_FILE, "r");

	if (fp == NULL) {
		for (size_t i = 0; i < sizeof(parallel); i++) {
			Pin p;
			p.label = malloc(sizeof(char) * 6);

			char tmp = i + 2 + '0';
			strncpy(p.label, DEFAULT_LABEL, 4);
			strncat(p.label, &tmp, 1);
			p.label[5] = 0;

			// TODO: Replace with port state (inb())
			p.state = OFF;

			parallel[i] = p;
		}
		return;
	}

	fclose(fp);
}

void write_to_file() {

}

int get_port_state() {
	// return inb(PORT)
	return 0;
}

const char* pin_to_json(Pin *p) {
	json_object* obj = json_object_new_object();

	json_object* label = json_object_new_string(p->label);
	json_object_object_add(obj, "label", label);

	json_object* state = json_object_new_int(p->state);
	json_object_object_add(obj, "state", state);

	return json_object_to_json_string(obj);
}