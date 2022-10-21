#pragma once

#include <vector>
#include "Arg.hpp"

class Args {

private:
	std::vector<Arg> argv;

public:
	Args(int agrc, char** argv);

	bool has(std::string arg);
	int indexOf(std::string arg);
	size_t count();

	Arg operator[] (int i);
	Arg operator[] (size_t i);
};