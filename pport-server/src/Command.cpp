#include "Command.h"

#include <fstream>
#include <sys/io.h>
#include <unistd.h>

#include <iostream>

#define PORT 0x378

Command::Command(std::string cmd) {
	std::smatch match;
	this->cmd = ToUpper(cmd);
	if (std::regex_search(this->cmd, match, RE_CMD)) {
		this->ints = match[1].str();
		this->pin = match[2].str() != "" ? match[2].str() == "ALL" ? 0xFF : std::stoi(match[2].str()) : 0;
		this->state = match[3].str() != "" && match[3].str() == "HIGH";
	}
	else {
		throw std::runtime_error("Invalid Command");
	}
}

unsigned int Command::Read() {
	return inb(PORT);
}

void Command::Write(int pin, bool state) {
	pin -= 2;
	unsigned int d;

    if (state) {
        d = 1 << pin;
        outb(inb(PORT) | d, PORT);
    }
    else {
        d = 0xFF - (1 << pin);
        outb(inb(PORT) & d, PORT);
    }

	std::ofstream f("/etc/pport.state");

	f << std::hex << this->Read();

	f.close();
}

void Command::Write(int data) {

	outb(data, PORT);

	std::ofstream f("/etc/pport.state");

	f << std::hex << this->Read();

	f.close();
}

std::string Command::FmtRead() {
	std::string result("[");
	unsigned int data = this->Read();

	for (size_t i = 0; i < 8; i++) {
		size_t d = 1 << i;
		if (d == 256) d--;

		if (i != 0) result.push_back(',');

		result += this->FmtPin(i + 2, data);
	}

	result.push_back(']');

	return result;
}

std::string Command::FmtPin(int pin, unsigned int data) {
	if (data == static_cast<unsigned int>(EOF)) {
		data = this->Read();
	}
	size_t d = 1 << (pin - 2);

	return "{\"pin\":" + std::to_string(pin) + ",\"state\":" + ((data & d) == d ? "true" : "false") + '}';
}

std::string Command::execute() {
	if (this->ints == "SHOW") {
		if (this->pin != 0) {
			return this->FmtPin(pin);
		}
		return this->FmtRead();
	}
	else if (this->ints == "SET") {
		if (this->pin == 0xFF) {
			this->Write(this->state ? 0xFF : 0);
			return this->cmd;
		}
		this->Write(this->pin, this->state);
		return this->cmd;
	}
	else if (this->ints == "REBOOT") {
		std::string s;
		std::ifstream f("/etc/pport.state");

		getline(f, s);

		f.close();

		this->Write(std::stoi(s, 0, 16));

		return "";
	}

	throw std::runtime_error("Error executing this command");
}

std::string Command::ToUpper(std::string s) {
	std::string result("");

	for (size_t i = 0; i < s.size(); i++) {
		if (s[i] >= 'a' && s[i] <= 'z')
			result.push_back(s[i] ^ 0x20);
		else {
			result.push_back(s[i]);
		}
	}

	return result;
}