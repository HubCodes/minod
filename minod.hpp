#ifndef _MINOD_H
#define _MINOD_H

#include <memory>

class Server;

class Minod final {
public:
	explicit Minod(const std::unique_ptr<Server> server);
	~Minod();

	void run();
private:
	std::unique_ptr<Server> server;
};

#endif
