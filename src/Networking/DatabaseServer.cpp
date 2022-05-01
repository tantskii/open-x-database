#include <omx/DatabaseServer.h>

#include "DatabaseServerImpl.h"

namespace omx {

	DatabaseServer::DatabaseServer(
		uint16_t port, uint32_t numThreads, std::string databaseDir, omx::Options options)
		: m_impl(port, numThreads, std::move(databaseDir), options)
	{}

	void DatabaseServer::start() {
		m_impl->start();
	}

	void DatabaseServer::stop() {
		m_impl->stop();
	}

	void DatabaseServer::wait() {
		m_impl->wait();
	}

	DatabaseServer::~DatabaseServer() = default;
}
