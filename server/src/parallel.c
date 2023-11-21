#include "parallel.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/io.h>
#include <libconfig.h>

#define READ_BUF 512
#define DEFAULT_LABEL "Pin "

#define MIN(a,b) (a > b ? b : a)

void load_parallel_from_file(Pin parallel[8]) {
	FILE *fp = fopen(STATE_FILE, "r");

	if (fp == NULL) {
		uint8_t current_value = inb(PORT);

		for (int i = 0; i < 8; i++) {
			Pin p;

			memcpy(p.label, DEFAULT_LABEL, 4);
			p.label[4] = i + 2 + '0';
			p.label[5] = 0;

			p.state = (current_value & (1 << i)) != 0 ? ON : OFF;

			parallel[i] = p;
		}

		write_to_file(parallel);
	}
	else {
		uint8_t value = 0;

		config_t cfg;
		config_setting_t *pins;
		config_init(&cfg);

		if (config_read(&cfg, fp) == CONFIG_FALSE) {
			config_destroy(&cfg);
			return;
		}

		pins = config_setting_get_member(cfg.root, "pins");

		if (!pins) {
			pins = config_setting_add(cfg.root, "pins", CONFIG_TYPE_LIST);
		}

		size_t count = (size_t)config_setting_length(pins);

		for (size_t i = 0; i < count; i++) {
			config_setting_t *p = config_setting_get_elem(pins, i);

			const char *lbl;
			int state;

			if (!(config_setting_lookup_string(p, "label", &lbl) && config_setting_lookup_bool(p, "state", &state))) {
				perror("Failed to lookup data for pin");
				continue;
			}

			Pin pin;

			size_t str_len = strlen(lbl);

			memcpy(pin.label, lbl, str_len);

			pin.state = state;

			value |= (1 << i) * pin.state;

			parallel[i] = pin;
		}
		outb( value, PORT );
		config_destroy(&cfg);
	}

	fclose(fp);
}

void write_to_file(Pin parallel[8]) {
	config_t cfg;
	config_setting_t *pins, *pin, *setting;

	config_init(&cfg);

	pins = config_setting_add(cfg.root, "pins", CONFIG_TYPE_LIST);
	
	for (size_t i = 0; i < 8; i++) {
		pin = config_setting_add(pins, NULL, CONFIG_TYPE_GROUP);

		setting = config_setting_add(pin, "label", CONFIG_TYPE_STRING);
		config_setting_set_string(setting, parallel[i].label);

		setting = config_setting_add(pin, "state", CONFIG_TYPE_BOOL);
		config_setting_set_bool(setting, parallel[i].state);
	}

	if (!config_write_file(&cfg, STATE_FILE)) {
		perror("Error writing file");
	}

	config_destroy(&cfg);
}

void *pin_to_mem(Pin *p, size_t *len) {
	// Allocate 1 more byte for terminator, plus 1 more for "is array"
	size_t data_len = sizeof(p->state) + sizeof(p->label) + 2;

	void *buf = malloc(data_len);

	if (buf == NULL) {
		return NULL;
	}

	memset(buf, 0, 1);
	memcpy(buf + 1, &(p->state), sizeof(p->state));
	memcpy(buf + 1 + sizeof(p->state), p->label, strlen(p->label) + 1);

	if (len != NULL) {
		*len = data_len;
	}

	return buf;
}

void *parallel_to_mem(Pin parallel[8], size_t *len) {
	size_t one_max_len = 261;
	size_t total_len = 1;

	void *buf = malloc(one_max_len * 8 + 1);

	if (buf == NULL) {
		return NULL;
	}

	memset(buf, 1, 1);

	for (size_t i = 0; i < 8; i++) {
		
		size_t p_len = 1;
		void *p_buf = pin_to_mem(&parallel[i], &p_len);

		memcpy(buf + total_len, p_buf + 1, p_len - 1);

		total_len += p_len - 1;
		free(p_buf);
	}

  if (len != NULL) {
    *len = total_len;
  }

	return buf;
}
