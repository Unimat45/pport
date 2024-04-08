#include "Server.h"
#include "pio.h"
#include <iostream>

int main(void) {
    if (io_init()) {
        std::cerr << "ERROR: Invalid permission. Try running this as sudo" << std::endl;
        return 1;
    }

    Server s(5663);
    s.startServer();
    return 0;
}
