#include "Server.h"

#include <thread>
#include <string>

#define MAX_BUF 1024

Server::Server(int port) {
	this->port = port;
}

void Server::StartServer() {
	server_fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (server_fd < 0) {
		throw std::runtime_error("server_fd");
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	memset(&client_addr, 0, sizeof(client_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(this->port);

	if ( bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0 ) {
		throw std::runtime_error("bind failed");
	}

	this->start();
}

void Server::StopServer() {
	this->running = false;
}

void Server::Run() {
	running = true;
	char buf[MAX_BUF];

	int n;
	socklen_t len = sizeof(client_addr);

	while ( running ) {
		n = recvfrom(server_fd, (char*)buf, MAX_BUF, MSG_WAITALL, (struct sockaddr*)&client_addr, &len);
		buf[n] = '\0';

		std::unique_ptr<Command> c = std::make_unique<Command>(buf);

		try {
			std::string result = c->Execute();
			sendto(server_fd, (const char*)result.c_str(), result.size(), MSG_CONFIRM, (const struct sockaddr*)&client_addr, len);
		}
		catch (std::exception& e) {
			sendto(server_fd, e.what(), strlen(e.what()), MSG_CONFIRM, (const sockaddr*)&client_addr, len);
		}
	}

	shutdown(server_fd, SHUT_RDWR);
}

void Server::start() {
	std::jthread(&Server::Run, this);
}