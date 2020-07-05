#include "server.hpp"

#include <cerrno>
#include <vector>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

static sockaddr_in make_address(
	const std::string& host,
	const unsigned short port);

class ServerImpl final {
public:
	explicit ServerImpl(
		const std::vector<std::shared_ptr<CmdProcessor>>& cmd_processors,
		const std::string& host,
		const unsigned short port,
		const int tcp_backlog);
	~ServerImpl();

	ServerError listen();
private:
	const std::vector<std::shared_ptr<CmdProcessor>>& cmd_processors;
	const std::string& host;
	const unsigned short port;
	const sockaddr_in address;
	const int tcp_backlog;
};

ServerImpl::ServerImpl(
	const std::vector<std::shared_ptr<CmdProcessor>>& cmd_processors,
	const std::string& host,
	const unsigned short port,
	const int tcp_backlog):
	cmd_processors(cmd_processors),
	host(host),
	port(port),
	address(make_address(host, port)),
	tcp_backlog(tcp_backlog) {}

ServerError ServerImpl::listen() {
	auto sockfd{ socket(PF_INET, SOCK_STREAM, 0) };
	if (sockfd < 0) {
		return ServerError(errno);
	}
	if (bind(sockfd, (sockaddr*)&address, sizeof(address)) < 0) {
		return ServerError(errno);
	}
	if (::listen(sockfd, tcp_backlog) < 0) {
		return ServerError(errno);
	}

	for (;;) {
		for (auto cmd_processor : cmd_processors) {
			cmd_processor->apply(cmd);
		}
	}
}

static sockaddr_in make_address(
	const std::string& host,
	const unsigned short port) {
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(host.c_str());

	return addr;
}
