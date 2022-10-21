#include "Arg.hpp"
#include <cmath>

Arg::Arg() {
	this->arg = "";
}

Arg::Arg(char* arg) {
	this->arg = arg;
}

Arg::Arg(std::string arg) {
	this->arg = arg;
}

bool Arg::isInt() {
	for(size_t i = 0; i < this->arg.size(); i++) {
		int cara = this->arg[i];
		if (cara < 48 || cara > 57) return false;
	}
	return true;
}

bool Arg::isBool() {
	return this->arg == "true" || this->arg == "1" || this->arg == "false" || this->arg == "0";
}

bool Arg::isDouble() {
	for (size_t i = 0; i < this->arg.size(); i++) {
		int cara = this->arg[i];
		if ((cara < 48 || cara > 57) && cara != 46) return false;
	}
	return true;
}

int Arg::ToInt() {
	int result = 0;
	bool neg = false;
	for (size_t i = 0; i < this->arg.size(); i++) {
		if (this->arg[i] == '-') {
			neg = true;
			continue;
		}
		result += (this->arg[i] - '0') * pow(10, this->arg.size() - i - 1);
	}
	return result * (neg ? -1 : 1);
}

bool Arg::ToBool() {
	return this->arg == "true" || this->arg == "1";
}

double Arg::ToDouble() {
	double result = 0;
	bool neg = false;
	for (size_t i = 0; i < this->arg.size(); i++) {
		int cara = this->arg[i];
		if (cara == '-') {
			neg = true;
			continue;
		}
		if (cara == 46) continue;
		int found = this->arg.find(".") - i;
		int cond = found < 0 ? 0 : -1;
		result += (cara - '0') * pow(10, found + cond);
	}

	int size = this->arg.substr(this->arg.find(".")).size();

	return round(result * pow(10, size)) / pow(10, size) * (neg ? -1 : 1);
}

std::string Arg::ToString() {
	return this->arg;
}

bool Arg::operator==(std::string arg2) {
	return this->arg == arg2;
}

bool Arg::operator!=(std::string arg2) {
	return this->arg != arg2;
}

std::ostream& Arg::operator<<(std::ostream& os)
{
	os << this->arg;
	return os;
}