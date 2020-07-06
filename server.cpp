#include "server.hpp"

#include <cerrno>
#include <utility>
#include <vector>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

static sockaddr_in make_address(
	const std::string& host,
	const unsigned short port);

static int recv_all(int fd, char* buffer, int size, int flags);

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
	auto sockfd = SocketFile(socket(PF_INET, SOCK_STREAM, 0));
	if (sockfd < 0) {
		return ServerError(errno);
	}
	if (bind(
		sockfd.get(),
		reinterpret_cast<const sockaddr*>(&address),
		sizeof(address)) < 0) {
		return ServerError(errno);
	}
	if (::listen(sockfd.get(), tcp_backlog) < 0) {
		return ServerError(errno);
	}

	for (;;) {
		int recv_errno{ 0 };
		SocketFile clientfd{ -1 };
		socklen_t client_addr_size;
		sockaddr client_addr;
		auto header{ std::make_unique<Cmd::Header>() };
		std::unique_ptr<Cmd> cmd{ std::make_unique<Cmd>() };
		std::unique_ptr<Result> result{ nullptr };

		if ((clientfd = accept(
			sockfd.get(),
			&client_addr,
			&client_addr_size)) < 0) {
			return ServerError(errno);
		}

		if (clientfd > 0 && recv_all(
			clientfd.get(),
			reinterpret_cast<char*>(header.get()),
			sizeof(header),
			0) < 0) {
			recv_errno = errno;
			if (recv_errno == ECONNRESET) {
				clientfd = -1;
				continue;
			}
			return ServerError(recv_errno);
		}

		cmd->header = std::move(header);

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

static int recv_all(int fd, char* buffer, int size, int flags) {
	int ret;

	while (size != 0 && (ret = recv(fd, (void*)buffer, size, flags)) != 0) {
		if (ret < 0) {
			return ret;
		}
		size -= ret;
		buffer += ret;
	}

	return 0;
}
