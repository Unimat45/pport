#include "Server.h"

Server::Server(int port) {
	this->port = port;
}

void Server::StartServer() {
	int opt = 1;
	server_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (server_fd == 0) {
		throw std::runtime_error("server_fd");
	}

	if ( setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) ) {
		throw std::runtime_error("setsocketopt");
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(this->port);

	if ( bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0 ) {
		throw std::runtime_error("bind failed");
	}

	if ( listen(server_fd, 3) < 0 ) {
		throw std::runtime_error("listen error");
	}

	this->addrlen = sizeof(address);
	
	this->start();
}

void Server::StopServer() {
	this->running = false;
}

void Server::Run() {
	bool running = true;

	std::cout << "Server started" << std::endl;

	while ( running ) {
		int socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

		if (socket < 0) {
			throw std::runtime_error("accept error");
		}

		std::cout << "Received Connection: ";

		Handler* handler = new Handler(socket);

		handler->start();
	}

	shutdown(server_fd, SHUT_RDWR);

	std::cout << "Server stopped" << std::endl;
}

void Server::start() {
	std::jthread(&Server::Run, this);
}