#ifndef TOKSTR_H
#define TOKSTR_H

#include <stddef.h>

char** tokstr(char* str, const char* delim, size_t* size);

#endif // TOKSTR_H