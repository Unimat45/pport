#include "Args.hpp"

Args::Args(int argc, char** argv) {
	for (int i = 1; i < argc; i++) {
		this->argv.push(Arg(argv[i]));
	}
}

bool Args::has(std::string arg) {
	for (size_t i = 0; i < this->argv.size(); i++) {
		if (this->argv[i] == arg) return true;
	}
	return false;
}

int Args::indexOf(std::string arg) {
	for (int i = 0; i < this->argv.size(); i++) {
		if (this->argv[i] == arg) return i;
	}
	return -1;
}

Arg Args::operator[](int i) {
	return this->argv[i];
}

Arg Args::operator[](size_t i) {
	return this->argv[i];
}