#include "Server.h"

#include <iostream>

namespace omx {

	Server::Server(uint16_t port, uint32_t numThreads, std::string databaseDir, omx::Options options)
		: ServerBase(numThreads)
		, m_port(port)
		, m_acceptor(std::make_shared<Database>(), m_service, m_port)
		, m_databaseDir(std::move(databaseDir))
		, m_databaseOptions(options)
	{}

	bool Server::init() {
		m_acceptor.start();

		auto database = m_acceptor.getDatabasePtr();

		database->open(m_databaseDir.c_str(), m_databaseOptions);

		std::cout
			<< "\nOMXDB Server started with the following params: "
			<< "\n\tPort               : " << m_port
			<< "\n\tThreads            : " << m_numThreads
			<< "\n\tDatabase directory : " << m_databaseDir;

		return true; // TODO: check whether start() is successful.
	}

	bool Server::close() {
		m_acceptor.stop();

		return true; // TODO: check whether stop() is successful.
	}

	Server::~Server() {
		stop();
	}
}
