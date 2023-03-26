#include "Client.hpp"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>

#define MAX_BUF 1024

Client::Client(std::string ip, int port) {
	this->ip = ip;
	this->port = port;

	this->initialize();
}

Client::~Client() {
	close(this->client_fd);
}

void Client::initialize() {
	if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		throw std::runtime_error("Error creating socket");
	}

	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(this->port);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
}

void Client::Write(std::string message) {
	sendto(client_fd, message.c_str(), message.size(), MSG_CONFIRM, (const struct sockaddr*)&serv_addr, sizeof(serv_addr));
}

std::string Client::Read() {
	char buf[MAX_BUF];
	socklen_t len;

	int n = recvfrom(client_fd, (char*)buf, MAX_BUF, MSG_WAITALL, (struct sockaddr*)&serv_addr, &len);
	buf[n] = '\0';

	return std::string(buf);
}

void Client::Close() {
	close(this->client_fd);
}