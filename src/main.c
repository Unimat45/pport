#include <stdio.h>
#include "command.h"

int main(void) {
    Pin p;
    p.label = "Piscine";
    p.state = ON;

    const char *buf = pin_to_json(&p);

    printf("%s\n", buf);

    return 0;    
}