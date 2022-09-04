#include <sys/socket.h>
#include <netinet/in.h>

#include "Handler.h"

class Server {
private:
	bool running = false;
	int port;
	int addrlen;
	struct sockaddr_in address;
	int server_fd;

public:
	Server(int port);

	void StartServer();
	void Run();
	void start();
	void StopServer();
};