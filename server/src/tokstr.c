#include <stdlib.h>
#include <string.h>

#define MAX_ITER 15

char *__tokstr(const char *str, const char delim) {
	static char *input = NULL;
	
	if (str != NULL) {
		input = str;
	}

	if (input == NULL) {
		return NULL;
	}

	char *result = malloc(strlen(input) + 1);

	int i = 0;

	while (input[i]) {
		if (input[i] != delim) {
			result[i] = input[i];
		}
		else {
			result[i] = 0;
			input = input + i + 1;

			return result;
		}

		i++;
	}

	result[i] = 0;
	input = NULL;

	return result;
}

size_t tokstr(char** result, char* str, const char* delim) {
	if (result == NULL) {
		return NULL;
	}

	size_t delim_len = strlen(delim);

	char* f = strstr(str, delim);

	if (f == NULL) {
		result[0] = str;
		return 1;
	}

	char* old = str;

	unsigned char iter = 0;
	
	while (f != NULL && iter < MAX_ITER) {
		size_t len = f - old;

		result[iter] = malloc(len);

		if (result[iter] == NULL) {
			return iter;
		}

		memcpy(result[iter], old, len);
		result[iter++][len] = 0;

		old = f + delim_len;
		f = strstr(old, delim);
	}

	size_t len = strlen(old);

	result[iter++] = malloc(len);

	if (result[iter] == NULL) {
		return 0;
	}

	memcpy(result[iter], old, len);

	return iter;
}
