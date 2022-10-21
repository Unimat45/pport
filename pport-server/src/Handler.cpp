#include "Handler.h"

#include <thread>

Handler::Handler(int socket) {
	this->socket = socket;
}

void Handler::run() {
	char buf[50] = { 0 };

	int valread = read(socket, buf, sizeof(buf));

	if (valread == -1) throw;

	/* HANDLER */
	try {
		Command cmd( buf );

		std::string data = cmd.Execute();

		write(socket, data.c_str(), data.size());
	}
	catch (std::exception& e) {
		write(socket, e.what(), strlen(e.what()));
	}
	/* END HANDLER */
	
	close(socket);

	std::cout << "Connection closed" << std::endl;
}

void Handler::start() {
	std::jthread(&Handler::run, this);
}