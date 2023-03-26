#include <netinet/in.h>

#include "Command.h"

class Server {
private:
	bool running = false;
	int port;
	struct sockaddr_in serv_addr, client_addr;
	int server_fd;

public:
	Server(int port);

	void StartServer();
	void Run();
	void start();
	void StopServer();
};