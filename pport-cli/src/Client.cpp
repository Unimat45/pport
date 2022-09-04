#include "Client.hpp"

#include <iostream>
#include <unistd.h>
#include <sys/socket.h>

Client::Client(std::string ip, int port) {
	this->ip = ip;
	this->port = port;

	this->initialize();
}

Client::~Client() {
	close(this->client_fd);
}

void Client::initialize() {

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		throw std::runtime_error("Error creating socket");
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(this->port);

	if (inet_pton(AF_INET, this->ip.c_str(), &serv_addr.sin_addr) <= 0) {
		throw std::runtime_error("Invalid address");
	}

	if ((client_fd = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) {
		throw std::runtime_error("Connection failed");
	}
}

void Client::Write(std::string message) {
	send(this->sock, message.c_str(), message.size(), 0);
}

std::string Client::Read() {
	char buf[512] = { 0 };

	read(this->sock, buf, 512);

	return std::string(buf);
}

void Client::Close() {
	close(this->client_fd);
}