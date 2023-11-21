#include <stdlib.h>
#include <string.h>

#define MAX_ITER 15

char *tokstr(char *str, const char delim) {
	static char *input = NULL;
	
	if (str != NULL) {
		input = str;
	}

	if (input == NULL) {
		return NULL;
	}

	char *result = malloc(strlen(input) + 1);

	if (result == NULL) {
		return NULL;
	}

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
