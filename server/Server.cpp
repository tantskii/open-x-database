#include "Server.h"

#include <boost/log/trivial.hpp>

namespace omx {

	Server::Server(uint16_t port, uint32_t numThreads)
		: ServerBase(numThreads)
		, m_port(port)
		, m_acceptor(std::make_shared<Database>(), m_service, m_port)
	{}

	bool Server::init() {
		m_acceptor.start();

		auto database = m_acceptor.getDatabasePtr();

		database->open("/tmp/omxdb");

		BOOST_LOG_TRIVIAL(info) << __PRETTY_FUNCTION__
			<< " Server Started. Listening to port " << m_port;

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
