#include <string>
#include <arpa/inet.h>

class Client {
private:
	std::string ip;
	int port;
	int client_fd;
	struct sockaddr_in serv_addr;

	void initialize();

public:
	Client(std::string ip, int port);
	~Client();

	void Write(std::string message);
	std::string Read();

	void Close();
};

