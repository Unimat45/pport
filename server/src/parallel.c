#include "parallel.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/io.h>

#define READ_BUF 512
#define DEFAULT_LABEL "Pin "

#define MIN(a,b) (a > b ? b : a)

char* read_file(FILE *fp, size_t* len) {
	if (fp == NULL) {
		return NULL;
	}

	fseek(fp, 0l, SEEK_END);
	*len = ftell(fp);
	rewind(fp);

	char *buf = malloc(*len + 1);
	*len = fread(buf, 1, *len, fp);

	buf[*len] = 0;
	
	return buf;
}

void load_parallel_from_file(Pin parallel[8]) {
	FILE *fp = fopen(STATE_FILE, "r");

	if (fp == NULL) {
		uint8_t current_value = inb(PORT);

		for (int i = 0; i < 8; i++) {
			Pin p;

			memcpy(p.label, DEFAULT_LABEL, 4);
			p.label[4] = i + 2 + '0';
			p.label[5] = 0;

			p.state = (current_value & (1 << i)) & 1;

			parallel[i] = p;
		}

		write_to_file();
	}
	else {
		size_t file_len;
		char *json = read_file(fp, &file_len);
		uint8_t value = 0;	

		json_object *data = json_tokener_parse(json);
		free(json);

		json_object* pins = json_object_object_get(data, "pins");

		for (size_t i = 0; i < 8; i++) {
			json_object * p = json_object_array_get_idx(pins, i);

			json_object* lbl =  json_object_object_get(p, "label");
			json_object* state =  json_object_object_get(p, "state");

			Pin pin;

			const char *tmp = json_object_get_string(lbl);
			size_t str_len = json_object_get_string_len(lbl) + 1;

			memcpy(pin.label, tmp, MIN(str_len, 260));

			pin.state = json_object_get_int(state);

			value |= (1 << i) * pin.state;

			parallel[i] = pin;
		}
	}
	outb( value, PORT );

	fclose(fp);
}

json_object* pin_to_json(Pin *p) {
	json_object* obj = json_object_new_object();

	json_object* label = json_object_new_string(p->label);
	json_object_object_add(obj, "label", label);

	json_object* state = json_object_new_int(p->state);
	json_object_object_add(obj, "state", state);

	return obj;
}

void write_to_file(Pin parallel[8]) {
	json_object *root = json_object_new_object();
	json_object *pins = json_object_new_array();
	
	for (size_t i = 0; i < 8; i++) {
		json_object* p = pin_to_json(parallel[i]);

		json_object_array_add(pins, p);
	}
	json_object_object_add(root, "pins", pins);


	FILE *fp = fopen(STATE_FILE, "w");

	if (fp == NULL) {
		fprintf(stderr, "Error creating config file. Exiting...\n");
		exit(1);
	}

	fprintf(fp, "%s", json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY));
	fflush(fp);

	fclose(fp);
}

size_t pin_to_mem(void *buf, Pin *p) {
	// Allocate 1 more byte for terminator, plus 1 more for "is array"
	size_t data_len = sizeof(p->state) + strlen(p->label) + 2;

	// void *buf = malloc(data_len);

	if (buf == NULL) {
		return NULL;
	}

  	memset(buf, 0, 1);
  	memcpy(buf + 1, &(p->state), sizeof(p->state));
	memcpy(buf + 1 + sizeof(p->state), p->label, strlen(p->label) + 1);

	return data_len;
}

size_t parallel_to_mem(void *buf, Pin parallel[8]) {
	size_t one_max_len = 261;
	size_t total_len = 1;

	// void *buf = malloc(one_max_len * 8 + 1);

	if (buf == NULL) {
		return NULL;
	}

	memset(buf, 1, 1);

	for (size_t i = 0; i < 8; i++) {
		size_t p_len = 1;
		void *p_buf = pin_to_mem(parallel[i], &p_len);

		memcpy(buf + total_len, p_buf + 1, p_len - 1);

		total_len += p_len - 1;
		free(p_buf);
	}

	return total_len;
}
