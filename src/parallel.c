#include "parallel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READ_BUF 512

void free_parallel(void) {
	for (size_t i = 0; i < 8; i++) {
		Pin *p = parallel[i];

		if (p) {
			if (p->label) {
				free(p->label);
			}
			free(p);
		}
	}
}

char* read_file(FILE *fp, size_t* len) {
	if (fp == NULL) {
		return NULL;
	}

	fseek(fp, 0l, SEEK_END);
	*len = ftell(fp);
	rewind(fp);

	char *buf = malloc(*len + 1);
	fread(buf, 1, *len, fp);

	buf[*len] = 0;
	
	return buf;
}

#define DEFAULT_LABEL "Pin "

static void free_parallel_labels(void) {
    for (size_t i = 0; i < sizeof(parallel); i++) {
		if (parallel[i]->label) {
        	free(parallel[i]->label);
		}
    }
}

void load_parallel_from_file() {
	atexit(free_parallel_labels);

	FILE *fp = fopen(STATE_FILE, "r");

	atexit(free_parallel);

	if (fp == NULL) {
		for (int i = 0; i < 8; i++) {
			Pin *p = malloc(sizeof(Pin));
			p->label = malloc(sizeof(char) * 6);

			memcpy(p->label, "Pin ", 4);
			p->label[4] = i + 2 + '0';
			p->label[5] = 0;

			// TODO: Replace with port state (inb())
			p->state = OFF;

			parallel[i] = p;
		}

		write_to_file();

		return;
	}

	size_t file_len;
	char *json = read_file(fp, &file_len);

	json_object *data = json_tokener_parse(json);
	json_object* pins = json_object_object_get(data, "pins");

	for (size_t i = 0; i < sizeof(parallel); i++) {
		json_object * p = json_object_array_get_idx(pins, i);

		json_object* lbl =  json_object_object_get(p, "Label");
		json_object* state =  json_object_object_get(p, "State");

		Pin* pin = malloc(sizeof(Pin));
		pin->label = (char*)json_object_get_string(lbl);
		pin->state = json_object_get_int(state);

		parallel[i] = pin;
	}

	fclose(fp);
}

void write_to_file() {
	json_object *root = json_object_new_object();
	json_object *pins = json_object_new_array();
	
	for (size_t i = 0; i < 8; i++) {
		json_object* p = pin_to_json(parallel[i]);

		json_object_array_add(pins, p);
	}
	json_object_object_add(root, "pins", pins);

	json_object *value = json_object_new_int(get_port_state());
	json_object_object_add(root, "value", value);

	FILE *fp = fopen(STATE_FILE, "w");

	if (fp == NULL) {
		fprintf(stderr, "Error creating config file. Exiting...\n");
		exit(1);
	}

	fprintf(fp, "%s", json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY));
	fflush(fp);

	fclose(fp);
}

int get_port_state() {
	int result = 0;

	for (size_t i = 0; i < 8; i++) {
		result += parallel[i]->state << i;
	}

	return result;
}

json_object* pin_to_json(Pin *p) {
	json_object* obj = json_object_new_object();

	json_object* label = json_object_new_string(p->label);
	json_object_object_add(obj, "Label", label);

	json_object* state = json_object_new_int(p->state);
	json_object_object_add(obj, "State", state);

	return obj;
}

char* parallel_to_json() {
	json_object *arr = json_object_new_array();

	for (size_t i = 0; i < 8; i++) {
		json_object *p = pin_to_json(parallel[i]);

		json_object_array_add(arr, p);
	}
	
	return json_object_to_json_string_ext(arr, JSON_C_TO_STRING_PLAIN);
}