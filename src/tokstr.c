#include <stdlib.h>
#include <string.h>

#define MAX_ITER 15

char** tokstr(char* str, const char* delim, size_t* size) {
	char** result = malloc(sizeof(char) * 260 * MAX_ITER);

	if (result == NULL) {
		return NULL;
	}

	char* f = strstr(str, delim);

	if (f == NULL) {
		result[0] = str;
		*size = 1;
		return result;
	}

	char* old = str;

	unsigned char iter = 0;
	
	while (f != NULL && iter < MAX_ITER) {
		size_t len = f - old;

		char* buf = malloc(sizeof(char) * len);

		if (buf == NULL) {
			continue;
		}

		memcpy(buf, old, len);
		buf[len] = 0;

		result[iter++] = buf;

		old = f + 1;
		f = strstr(old, delim);
	}

	result[iter++] = old;
	*size = iter;
	return result;
}