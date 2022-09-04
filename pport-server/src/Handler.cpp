#include "Handler.h"

Handler::Handler(int socket) {
	this->socket = socket;
}

void Handler::run() {
	char buf[15] = { 0 };

	int valread = read(socket, buf, 15);

	if (valread == -1) throw;

	try {
		Command* cmd = new Command(buf);

		std::string data = cmd->execute();

		write(socket, data.c_str(), data.size());
	}
	catch (std::exception& e) {
		write(socket, e.what(), strlen(e.what()));
	}
	
	close(socket);

	std::cout << "Connection closed" << std::endl;
}

void Handler::start() {
	std::jthread(&Handler::run, this);
}