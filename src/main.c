#include <stdio.h>
#include "command.h"

int main(void) {
    load_parallel_from_file();

    const char *buf = pin_to_json(&parallel[0]);

    printf("%s\n", buf);

    return 0;    
}