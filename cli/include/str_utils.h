#ifndef STR_UTILS_H
#define STR_UTILS_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t state;
    const char *label;
} Pin;

size_t utf8_strlen(const char *s);
size_t read_line(char *const line);
int istrcmp(const char *a, const char *b);
size_t findLongestLabel(Pin **parallel, size_t size);

#endif
