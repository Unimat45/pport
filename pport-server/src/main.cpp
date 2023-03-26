#include <sys/io.h>
#include <fstream>

#include <unistd.h>

#include "Server.h"

#define SERVER_PORT 5663

bool exists(std::string filename) {
    return access(filename.c_str(), F_OK) != -1;
}

void handleReboot() {
    if (!exists(STATE_FILE)) {
        Json::Value file;

        file["value"] = 0;

        Json::Value labels;

        for (size_t i = 0; i < PORT_SIZE; i++) {
            Json::Value pin;
            pin["pin"] = i + 2;
            pin["label"] = Json::nullValue;

            labels.append(pin);
        }

        file["labels"] = labels;

        std::ofstream f(STATE_FILE);

        f << file;

        f.close();
    }
}

int main() {
    if (ioperm(PORT, 1, 1)) {
        std::cerr << "Access denied" << std::endl;
        return EXIT_FAILURE;
    }

    handleReboot();

    Server server = Server(SERVER_PORT);


	std::cout << "Server started on " << SERVER_PORT << std::endl;

    server.StartServer();

    return EXIT_SUCCESS;
}
