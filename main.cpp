#include "minod.hpp"
#include "server.hpp"

#include <cstdlib>
#include <memory>

int main(void) {
	auto server{ std::make_unique<Server>() };
	auto minod{ std::make_unique<Minod>(server) };

	minod->run();

	return EXIT_SUCCESS;
}
