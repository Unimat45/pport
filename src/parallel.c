#include "parallel.h"

#include <stdio.h>

#include "json-c/json_object.h"
#include "json-c/json_tokener.h"

void load_parallel_from_file() {
	FILE *fp = fopen(STATE_FILE, "r");

	if (fp == NULL) {
		for (size_t i = 0; i < sizeof(parallel); i++) {
			Pin p;
			p.label = "Pin " + (i + 2 + '0');
			// TODO: Replace with port state (inb())
			p.state = OFF;

			parallel[i] = p;
		}
		return;
	}

	
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