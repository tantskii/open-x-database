#include <iostream>

#include "Server.h"

int main() {
	const uint16_t port = 3132;
	const uint16_t numThreads = 4;
	const std::string databaseDirectory = "/tmp/omxdb";

	omx::Options databaseOptions = {};
	databaseOptions.maxMemTableSize  = 1 * 1024 * 1024;
	databaseOptions.maxWalBufferSize = 1 * 1024;
	databaseOptions.hashType         = omx::HashType::CityHash128;
	databaseOptions.compressionType  = omx::CompressionType::LZ4;

	auto server = omx::Server(port, numThreads, databaseDirectory, databaseOptions);

	server.start();
}