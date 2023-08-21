#include <stdio.h>
#include "command.h"

int main(void) {
    load_parallel_from_file();

    printf("%s\n", parallel_to_json());

    return 0;    
}