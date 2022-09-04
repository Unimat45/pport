#include <regex>

class Command {
private:
	static std::string ToUpper(std::string s);
	std::string cmd;
	std::string ints;
	int pin;
	bool state;

	std::regex RE_CMD = std::regex("^(SET|SHOW|REBOOT)(?: PIN ([2-9]|ALL) ?(HIGH|LOW)?)?$");

	unsigned int Read();
	void Write(int pin, bool state);
	void Write(int data);

	std::string FmtRead();
	std::string FmtPin(int pin, unsigned int data = EOF);

public:
	Command(std::string cmd);
	std::string execute();
};
