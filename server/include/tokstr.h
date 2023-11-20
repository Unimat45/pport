#ifndef TOKSTR_H
#define TOKSTR_H

#include <stddef.h>

char *__tokstr(char *str, const char delim);
size_t tokstr(char** result, char* str, const char* delim);

#endif // TOKSTR_H
