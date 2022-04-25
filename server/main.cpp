#include <iostream>

#include "Server.h"

int main() {
	uint16_t port = 3132;
	uint16_t numThreads = 4;

	auto server = omx::Server(port, numThreads);

	server.start();
}