#include "Acceptor.h"
#include "Session.h"

#include <boost/log/trivial.hpp>
#include <utility>

using namespace boost::asio::ip;

namespace omx {

	Acceptor::Acceptor(DatabasePtr database, boost::asio::io_service& service, uint16_t port)
		: m_database(std::move(database))
		, m_service(service)
		, m_acceptor(m_service, tcp::endpoint(boost::asio::ip::address_v4::any(), port))
		, m_isStopped(false)
	{}

	void Acceptor::start() {
		m_acceptor.listen();
		initAccept();
	}

	void Acceptor::stop() {
		m_isStopped.store(false);
	}

	void Acceptor::initAccept() {
		auto socket = std::make_shared<tcp::socket>(m_service);

		auto acceptCallback = [this, socket](const BoostError& error) {
			onAccept(error, socket);
		};

		m_acceptor.async_accept(*socket, acceptCallback);
	}

	void Acceptor::onAccept(const BoostError& errorCode, SocketPtr socket) {

		if (!errorCode) {
			auto* session = new Session(m_database, std::move(socket));

			// This session object will be deleted in session::onFinished()
			session->startHandling();
		}
		else {
			BOOST_LOG_TRIVIAL(error) << __PRETTY_FUNCTION__
				<< " Error code = " << errorCode.value()
				<< ". Message: " << errorCode.message();
		}

		// Init next async accept operation if acceptor has not been stopped yet.
		if (!m_isStopped) {
			initAccept();
		} else {
			m_acceptor.close();
		}
	}

	DatabasePtr Acceptor::getDatabasePtr() {
		return m_database;
	}

}
