#include "Command.h"

class Handler {

private:
	int socket;

public:
	Handler(int socket);

	void run();
	void start();
};