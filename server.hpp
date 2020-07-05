#ifndef _SERVER_HPP
#define _SERVER_HPP

#include "cmd_processor.hpp"

#include <memory>
#include <string>
#include <vector>

class ServerImpl;
class ServerError;

class Server final {
public:
	enum {
		DEFAULT_PORT = 7070,
	};
	explicit Server(
		const std::vector<std::shared_ptr<CmdProcessor>>& cmd_processors,
		const std::string& host,
		const unsigned short port = DEFAULT_PORT);
	~Server();

	ServerError listen();
private:
	std::unique_ptr<ServerImpl> impl;
};

class ServerError final {
public:
	explicit ServerError(int underlying_errno);
	~ServerError();

	const std::string to_string() const noexcept;
private:
	int underlying_errno;
};

#endif
