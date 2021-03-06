#include "DatabaseServerImpl.h"

#include <iostream>

namespace omx {

	DatabaseServerImpl::DatabaseServerImpl(uint16_t port, uint32_t numThreads, std::string databaseDir, omx::Options options)
		: ServerBase(numThreads)
		, m_port(port)
		, m_acceptor(std::make_shared<Database>(), m_service, m_port)
		, m_databaseDir(std::move(databaseDir))
		, m_databaseOptions(options)
	{}

	bool DatabaseServerImpl::init() {
		m_acceptor.start();

		auto database = m_acceptor.getDatabasePtr();

		database->open(m_databaseDir.c_str(), m_databaseOptions);

		std::cout
			<< "\nOMXDB Server started with the following params: "
			<< "\n\tPort               : " << m_port
			<< "\n\tThreads            : " << m_numThreads
			<< "\n\tDatabase directory : " << m_databaseDir
			<< std::endl;

		return true; // TODO: check whether start() is successful.
	}

	bool DatabaseServerImpl::close() {
		m_acceptor.stop();

		return true; // TODO: check whether stop() is successful.
	}

	DatabaseServerImpl::~DatabaseServerImpl() {
		stop();
	}
}
