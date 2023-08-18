#include <stdio.h>
#include "command.h"

int main(void) {
    Command* c = token_command("SHOW PIN ALL");

    if (c == NULL) {
        (void)printf("Failed to parse\n");
        return 1;
    }

    (void)printf("%d\n", c->instruction);
    (void)printf("%d\n", c->pin);

    free_command(c);

    return 0;    
}