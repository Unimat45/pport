#pragma once

#include <iostream>

class Arg {

private:
	std::string arg;

public:
	Arg();
	Arg(char* arg);
	Arg(std::string arg);

	bool isInt();
	bool isBool();
	bool isDouble();

	int ToInt();
	bool ToBool();
	double ToDouble();
	std::string ToString();

	bool operator==(std::string arg2);
	bool operator!=(std::string arg2);
	std::ostream& operator<<(std::ostream& os);
};